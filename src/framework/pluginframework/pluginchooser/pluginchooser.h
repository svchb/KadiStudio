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

#pragma once

#include <QDialog>
#include <QLabel>
#include <QListWidget>
#include <QPushButton>
#include <QCheckBox>
#include <QMessageBox>

#include <cpputils/dllapi.hpp>

#include "../pluginmanagerinterface.h"

class Plugin;

class ListWidgetItem : public QListWidgetItem {
  public:
    ListWidgetItem(const LibFramework::Plugin* plugin);
    virtual ~ListWidgetItem();

    QString getNamespacePath() const;
    const LibFramework::Plugin* getPlugin() const;

  private:
    const LibFramework::Plugin *plugin;
};

/**
 * @brief      A dialog for loading/unloading of plugins at runtime.
 * @ingroup    framework
 */
class DLLAPI PluginChooser : public QDialog {
  Q_OBJECT

  public:
    PluginChooser(QWidget* parent = nullptr);
    virtual ~PluginChooser();

    void show(const QString &namespacepath, bool multiselect);
    const QStringList& getSelection();

    template<typename T>
    T chooseFrom(LibFramework::PluginManagerInterface* pluginmanager, const QString &namespacepath) {
      auto allavailableinterfaces = pluginmanager->getInterfaces(namespacepath.toStdString());

      if (allavailableinterfaces.empty()) {
        QMessageBox::critical(this->parentWidget(), tr("No Plugin Found"), tr("No Plugin was found for namespace '%1'.").arg(namespacepath));
        return nullptr;
      }

      if (allavailableinterfaces.size() == 1) {
        return allavailableinterfaces.begin()->second->getInterface<T>();
      }

      // use plugin chooser here to select a single plugin to use
      // 'this' can be used as one would request an instance of PluginChooserFrameworkInterface
      this->show(namespacepath, false);
      QString selectedItem = this->getSelection().first();

      if (selectedItem.isEmpty()) {
        QMessageBox::critical(this->parentWidget(), tr("No Plugin Selected"), tr("No Plugin was selected for namespace '%1'.").arg(namespacepath));
        return nullptr;
      }

      return allavailableinterfaces[selectedItem.toStdString()]->getInterface<T>();
    }

  private Q_SLOTS:
    void reject() override;
    void accept() override;
    void populatePluginList(const QString& namespacepath);
    void changeButtonVisibility();

  private:
    QListWidget *listwidget;
    QLabel *lbldescription;
    QLabel *lblnamespace;
    QCheckBox *rememberDecision;
    QPushButton *applybutton;

    QStringList selectedNamespaces;

};
