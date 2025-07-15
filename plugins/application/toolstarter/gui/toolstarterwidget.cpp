/* Copyright 2025 Karlsruhe Institute of Technology
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License. */

#include <QDebug>
#include <QMenu>
#include <QRegularExpression>
#include <QVBoxLayout>
#include <QSplitter>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>

#include <properties/data/properties.h>
#include <properties/data/ambassador.h>
#include <properties/data/propertyfactory.h>

#include <plugins/infrastructure/toolchooser/toolchooserinterface.h>
#include <plugins/infrastructure/propertyformwidget/propertyformwidgetinterface.h>

#include "framework/dock/dockinterface.h"
#include "framework/dock/dockdelegate.h"
#include "framework/dock/dockwindow.h"
#include "toolprocessdialog.h"
#include "toolbookmarks.h"
#include "toolhistorydialog.h"
#include "toolstarterwidget.h"

ToolStarterWidget::ToolStarterWidget(LibFramework::PluginManagerInterface* pluginmanager, QWidget* parent)
    : QWidget(parent),
      pluginmanager(pluginmanager),
      dialog(nullptr) {

  setObjectName("Tool Starter Plugin");

  auto layout = new QVBoxLayout();

  QSplitter *splitter = new QSplitter();

  toolchooser_interface = pluginmanager->getInterface<ToolChooserInterface*>("/plugins/infrastructure/toolchooser")->clone();

  toolchooser_interface->setToolSelectedCallback([this] () {
    toolSelected();
  });
  toolchooser_interface->setToolResetCallback([this] () {
    toolReset();
  });
  splitter->addWidget(toolchooser_interface->getWidget());

  property_form_interface = pluginmanager->getInterface<PropertyFormWidgetInterface*>("/plugins/infrastructure/propertyformwidget");
  splitter->addWidget(property_form_interface->getWidget());

  layout->addWidget(splitter);

  auto toolstring_layout = new QHBoxLayout();

  toolhistorydialog = new ToolHistoryDialog();
  connect(toolhistorydialog, SIGNAL(toolChanged(QString)), this, SLOT(toolChanged(QString)));
  historybutton = new QPushButton(tr("History ..."));
  historybutton->setShortcut(QKeySequence("Ctrl+H"));
  connect(historybutton, &QPushButton::clicked, toolhistorydialog, &ToolHistoryDialog::exec);
  toolstring_layout->addWidget(historybutton);

  toolstring_widget = new QLineEdit;
  toolstring_widget->setReadOnly(true);
  toolstring_layout->addWidget(toolstring_widget);

  start_button = new QPushButton(tr("Start"));
  start_button->setEnabled(false);
  start_button->setToolTip(tr("Make sure all required parameters are set."));
  start_button->setShortcut(QKeySequence("Ctrl+S"));
  connect(start_button, &QPushButton::clicked, this, &ToolStarterWidget::startTool);
  toolstring_layout->addWidget(start_button);

  layout->addLayout(toolstring_layout);

  setLayout(layout);

  bookmarkmenu = createMenu();
}

ToolStarterWidget::~ToolStarterWidget() {
  delete toolhistorydialog;
  delete dialog;
  delete toolchooser_interface;
  delete tb;
}

QMenu* ToolStarterWidget::createMenu() {
  QMenu *menu = new QMenu(tr("&Bookmarks"));

  tb = new ToolBookmarks(this, menu, this->toolstring_widget);

  return menu;
}

QMenu* ToolStarterWidget::getMenu() {
  return bookmarkmenu;
}

/** @brief execute tool within the plugin
  */
void ToolStarterWidget::startTool() {
  toolhistorydialog->addToolToHistory(toolstring_widget->text(), QDateTime::currentDateTime());

  QStringList parameters;
  for (const ToolParameter &toolparameter : parametervector) {
    for (const QString& argument : toolparameter.arguments()) {
      parameters << argument;
    }
  }

  DockInterface *dock = DockDelegate::getInstance();
  DockWindow *dockwindow = new DockWindow();
  dialog = new ToolProcessDialog(toolchooser_interface->getToolDescription().name(), parameters, this);
  dockwindow->addWidget(dialog, Qt::RightDockWidgetArea);
  dock->addDockWindow("/plugins/application/toolstarter", dockwindow);
  dockwindow->setFloating(true);
  dock->dockWindow("/plugins/application/toolstarter", dockwindow, false);
}

std::unique_ptr<Ambassador> ToolStarterWidget::buildToolArgumentsAmbassador(const QVector<ToolParameter>& target_vector) {
  std::unique_ptr<PropertiesModel> ambassador = std::make_unique<PropertiesModel>(toolchooser_interface->getToolDescription().name().toStdString());

  event_handles += ambassador->onDomChange([this]([[maybe_unused]] const std::string& path) {
    onToolArgumentsChanged();
  });

  for (const ToolParameter &toolparameter : target_vector) {
    const std::string default_value = toolparameter.getDefaultValue().toStdString();
    const std::string value = toolparameter.getValue().toStdString();
    const std::string description = toolparameter.getDescription().toStdString();
    const std::string property_name = toolparameter.getLongName().toStdString();
    bool isoptional = !toolparameter.isRequired() && (toolparameter.getType() != "flag");

    Property *property = PropertyFactory::createProperty(property_name, toolparameter.getType().toStdString(), isoptional, default_value);

    if (not value.empty()) {
      property->fromString(value);
    }

    ValueTypeInterfaceHint *hint = property->updateHint();
    hint->setDescription(description);
    hint->setEntry("optional", isoptional);
    hint->setEntry("optional_set", toolparameter.isEnabled() == "true");
    ambassador->addProperty(property);

    // set label
    std::string label_text = property_name;
    QRegularExpression re("arg[0-9]*");
    QRegularExpressionMatch match = re.match(QString::fromStdString(property_name));
    if (match.hasMatch()) {
      if (toolparameter.getType() == "filein") {
        label_text = "Input";
      } else if (toolparameter.getType() == "fileout") {
        label_text = "Output";
      }
    }
    hint->setEntry("label", label_text);
  }
  return ambassador;
}

void ToolStarterWidget::transferParametersToVector(QVector<ToolParameter>& target_vector, const Ambassador* ambassador) {
  const auto& properties = ambassador->getProperties();
  for (std::size_t i = 0; i < properties.size(); i++) {
    const Property *property = properties[i].get();
    const ValueTypeInterfaceHint *hint = property->getHint();
    QString data;
    bool requiredparameter = true;
    if (hint->hasEntry("optional")) {
      bool optional = false;
      hint->getEntry("optional", optional);
      requiredparameter = !optional;
    }
    bool typeflag = (property->getValueTypeInfo() == typeid(bool));

    QString value = QString::fromStdString(property->toString());

    if (requiredparameter || (typeflag && dynamic_cast<const BoolProperty*>(property)->getValue())) {
      data = value;
    } else {
      const std::string optional_set_key = "optional_set";
      if (hint->hasEntry(optional_set_key)) {
        bool is_set;
        hint->getEntry(optional_set_key, is_set);
        if (is_set) {
          data = value;
        }
      } else {
        qDebug() << "Property " << QString::fromStdString(property->toString()) << " is optional but does not specify optional_set -> assuming it is not set!";
      }
    }
    target_vector[i].setValue(data);
  }
}

void ToolStarterWidget::onToolArgumentsChanged() {
  QString toolstring = toolchooser_interface->getToolDescription().name();
  transferParametersToVector(parametervector, tool_ambassador.get());
  bool allrequired = true;
  Q_FOREACH (const ToolParameter &toolparameter, parametervector) {
    const QStringList &arguments = toolparameter.arguments();

    if (arguments.isEmpty()) {
      if (toolparameter.isRequired()) allrequired = false;
    } else {
      toolstring += " ";
      toolstring += arguments.join(" ");
    }
  }

  if (toolstring.isNull()) {
    qWarning() << tr("configuration from widget value interface failed");
    return;
  }

  toolstring_widget->setText(toolstring);
  start_button->setEnabled(allrequired);
}

void ToolStarterWidget::toolSelected() {
  property_form_interface->reset();

  parametervector = toolchooser_interface->getToolDescription().parameterVector();
  tool_ambassador = buildToolArgumentsAmbassador(parametervector);

  property_form_interface->createWidgets(tool_ambassador.get());
  onToolArgumentsChanged();
}

void ToolStarterWidget::toolReset() {
  property_form_interface->reset();
  toolstring_widget->setText("");
}

void ToolStarterWidget::toolChanged(QString toolidentificationstring) {
  toolchooser_interface->setTool(toolidentificationstring);
}
