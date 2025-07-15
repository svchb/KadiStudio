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

#include <QVBoxLayout>
#include <QDialog>
#include <QDialogButtonBox>
#include <QLabel>

#include <pluginframework/plugininfo/pluginstatus.h>
#include <pluginframework/pluginmanager.h>

#include "pluginchooser.h"

ListWidgetItem::ListWidgetItem(const LibFramework::Plugin* plugin) : plugin(plugin) {
}

ListWidgetItem::~ListWidgetItem() = default;

QString ListWidgetItem::getNamespacePath() const {
  return QString::fromStdString(plugin->getPluginInfo()->getNamespace());
}

const LibFramework::Plugin* ListWidgetItem::getPlugin() const {
  return plugin;
}

PluginChooser::PluginChooser(QWidget* parent) : QDialog(parent) {
  setWindowTitle("Pluginchooser");

  listwidget = new QListWidget();
  listwidget->setFocusPolicy(Qt::StrongFocus);
  listwidget->setSelectionMode(QAbstractItemView::MultiSelection);
  listwidget->setSelectionBehavior(QAbstractItemView::SelectRows);
  connect(listwidget, SIGNAL(itemSelectionChanged()), this, SLOT(changeButtonVisibility()));

  lbldescription = new QLabel("the text to show the user");
  lblnamespace = new QLabel("the namespace to load plugins from");

  QDialogButtonBox *buttonbox = new QDialogButtonBox(QDialogButtonBox::Cancel | QDialogButtonBox::Apply);

  applybutton = buttonbox->button(QDialogButtonBox::Apply);
  applybutton->setToolTip("Start or stop a plugin depending on its current state");
  applybutton->setDisabled(true);
  applybutton->setAutoDefault(true);
  applybutton->setDefault(true);
  connect(applybutton, &QPushButton::clicked, this, &PluginChooser::accept);

  QPushButton *cancelbutton = buttonbox->button(QDialogButtonBox::Cancel);
  cancelbutton->setAutoDefault(false);
  cancelbutton->setDefault(false);
  connect(cancelbutton, &QPushButton::clicked, this, &PluginChooser::reject);

  rememberDecision = new QCheckBox("Remember Choice");
  rememberDecision->setToolTip("Remembers the choice for this session only");
  rememberDecision->setVisible(false);

  QVBoxLayout *layout = new QVBoxLayout();
  layout->addWidget(lbldescription);
  //layout->addWidget(lblnamespace);
  layout->addWidget(listwidget);

  auto *hbox = new QHBoxLayout(nullptr);
  hbox->addWidget(rememberDecision);
  hbox->addWidget(buttonbox, 0, Qt::AlignRight);

  layout->addLayout(hbox);

  setLayout(layout);
}

PluginChooser::~PluginChooser() {
}

void PluginChooser::show(const QString &namespacepath, bool multiselect) {
  lblnamespace->setText(namespacepath);
  lbldescription->setToolTip("Plugins available in namespace: " + namespacepath);
  populatePluginList(namespacepath);

  if (multiselect) {
    //rememberDecision->setVisible(false);
    lbldescription->setText("Select the plugin(s) to load from the list below:");
    listwidget->setSelectionMode(QAbstractItemView::MultiSelection);
  } else {
    //rememberDecision->setVisible(true);
    lbldescription->setText("Select the plugin to use from the list below:");
    listwidget->setSelectionMode(QAbstractItemView::SingleSelection);
  }

  exec();
}

const QStringList& PluginChooser::getSelection() {
  return selectedNamespaces;
}

void PluginChooser::changeButtonVisibility() {
  if (listwidget->selectedItems().isEmpty()) {
    applybutton->setDisabled(true);
  } else {
    applybutton->setEnabled(true);
  }
}

void PluginChooser::reject() {
  selectedNamespaces.clear();
  selectedNamespaces << ""; // if the list is empty, put the empty string in to indicate that there is a problem
  listwidget->clearSelection();

  QDialog::reject();
}

void PluginChooser::accept() {
  selectedNamespaces.clear();
  for (QListWidgetItem *item : listwidget->selectedItems()) {
    selectedNamespaces << ((ListWidgetItem *) item)->getNamespacePath();
  }

  if (selectedNamespaces.empty()) {
    selectedNamespaces << ""; // if the list is empty, put the empty string in to indicate that there is a problem
  }

  QDialog::accept();
}

void PluginChooser::populatePluginList(const QString& namespacepath) {
  listwidget->clear();

  LibFramework::PluginManagerInterface *pluginmanager = LibFramework::PluginManager::getInstance();
  std::vector<const LibFramework::Plugin*> plugins = pluginmanager->getPlugins(namespacepath.toStdString());

  for (const LibFramework::Plugin *plugin : plugins) {
    const LibFramework::PluginInfo *plugininfo = plugin->getPluginInfo();
    QString pluginnamespace = QString::fromStdString(plugininfo->getNamespace());
    if (pluginnamespace.contains(namespacepath)) {
      QString icon = QString::fromStdString(plugininfo->getIcon());
      QString descriptionname = QString::fromStdString(plugininfo->getName());
      QString description = QString::fromStdString(plugininfo->getDescription());

      ListWidgetItem *item = new ListWidgetItem(plugin);
      QString a = descriptionname;
      QString b = description;
      item->setToolTip(description);
      item->setIcon(QIcon(icon));
      item->setSizeHint(QSize(150, 51));

      const LibFramework::PluginStatus *pluginstatus = plugin->getPluginStatus();
      if (pluginstatus->isLoaded()) {
        if (pluginstatus->isRunning()) {
          a += "   [RUNNING]";
        } else {
          a += "   [LOADED]";
        }
      } else {
        a += "   [UNLOADED]";
      }

      item->setText(a + "\n" + b);

      listwidget->addItem(item);
    }
  }
}
