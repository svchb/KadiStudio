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

#include <QScrollArea>
#include <QLabel>
#include <QToolBox>
#include <QDialogButtonBox>
#include <QLayout>
#include <QTextEdit>
#include <QPushButton>

#include <framework/pluginframework/pluginmanager.h>
#include <plugins/infrastructure/toolchooser/toolchooserinterface.h>
#include "toolhistorysettings.h"
#include "toolhistorydialog.h"

ToolHistoryDialog::ToolHistoryDialog() {

  setWindowTitle(tr("Execution History"));

  QVBoxLayout *dialoglayout = new QVBoxLayout;

  toolboxwidget = new QToolBox;

  auto scroll_area = new QScrollArea();
  scroll_area->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
  scroll_area->setWidgetResizable(true);
  scroll_area->setWidget(toolboxwidget);

  dialoglayout->addWidget(scroll_area);

  textedit = new QTextEdit;
  dialoglayout->addWidget(textedit);

  QDialogButtonBox *dialogbuttonbox = new QDialogButtonBox(QDialogButtonBox::Apply | QDialogButtonBox::Cancel);
  connect(dialogbuttonbox->button(QDialogButtonBox::Apply), &QPushButton::clicked, this, &ToolHistoryDialog::changeTool);
  connect(dialogbuttonbox->button(QDialogButtonBox::Cancel), &QPushButton::clicked, this, &ToolHistoryDialog::close);
  dialoglayout->addWidget(dialogbuttonbox);

  setLayout(dialoglayout);

  loadHistory();
}

ToolHistoryDialog::~ToolHistoryDialog() {
}

void ToolHistoryDialog::loadHistory() {
  ToolHistorySettings settings;
  QStringList recenttools = settings.load();

  Q_FOREACH (const QString &recenttoolsstring, recenttools) {
    if (recenttoolsstring.contains(";")) {
      QStringList entries = recenttoolsstring.split(";");
      QString toolidentificationstring = entries.at(0);
      QDateTime datetime = QDateTime::fromString(entries.at(1), Qt::ISODate);

      addTool(toolidentificationstring, datetime);
    }
  }
  toolboxwidget->setCurrentIndex(0);

  bool nohistory = (toolboxwidget->count() == 0);

  if (nohistory) {
    toolboxwidget->hide();
    showHelpText();
  } else {
    toolboxwidget->show();
    textedit->hide();
  }
}

void ToolHistoryDialog::addTool(const QString& toolidentificationstring, const QDateTime& datetime) {
  if (toolboxwidget->isHidden()) {
    toolboxwidget->show();
    textedit->hide();
  }

  const ToolDescription &tooldescription = LibFramework::PluginManager::getInstance()->getInterface<ToolChooserInterface*>("/plugins/infrastructure/toolchooser")->getToolDescription(toolidentificationstring);
  const QVector<ToolParameter> &parametervector = tooldescription.parameterVector();

  QLabel *optionalheaderlabel = new QLabel("<b>"+tr("Optional")+"</b>");
  QLabel *nameheaderlabel = new QLabel("<b>"+tr("Name")+"</b>");
  QLabel *valueheaderlabel = new QLabel("<b>"+tr("Value")+"</b>");

  QGridLayout *gridlayout = new QGridLayout;
  gridlayout->addWidget(optionalheaderlabel, 0, 0);
  gridlayout->addWidget(nameheaderlabel, 0, 1);
  gridlayout->addWidget(valueheaderlabel, 0, 2);

  int row = 0;

  Q_FOREACH (ToolParameter toolparameter, parametervector) {
    bool parameteroptional = (!toolparameter.isRequired());
    QString longname = toolparameter.getLongName();

    row++;

    QLabel *name = new QLabel(longname);
    gridlayout->addWidget(name, row, 1);

    QLabel *value = new QLabel(toolparameter.getDefaultValue());
    gridlayout->addWidget(value, row, 2);

    if (parameteroptional) {
      QLabel *optional = new QLabel(QString(toolparameter.isEnabled()));
      gridlayout->addWidget(optional, row, 0);
    }
  }

  QWidget *contentwidget = new QWidget;
  contentwidget->setLayout(gridlayout);

  toolboxwidget->insertItem(0, contentwidget, tooldescription.shortName() + " " + datetime.toString());

  toolidentificationstrings.insert(0, toolidentificationstring);
}

void ToolHistoryDialog::addToolToHistory(const QString& toolidentificationstring, const QDateTime& datetime) {
  ToolHistorySettings settings;
  settings.save(toolidentificationstring, datetime);

  addTool(toolidentificationstring, datetime);
  toolboxwidget->setCurrentIndex(0);
}

void ToolHistoryDialog::changeTool() {
  int index = toolboxwidget->currentIndex();
  QString toolidentificationstring = toolidentificationstrings.at(index);

  Q_EMIT toolChanged(toolidentificationstring);
  QDialog::accept();
}

void ToolHistoryDialog::showHelpText() {
  textedit->setText(tr("No history found.\n\nConfigure and start a tool to add it to the history.\n\nThe configuration will then be available the next time the dialog is openend."));
  QPalette p;
  p.setColor(QPalette::Text, Qt::gray);
  textedit->setPalette(p);
}
