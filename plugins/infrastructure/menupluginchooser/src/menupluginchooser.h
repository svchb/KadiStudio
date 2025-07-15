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

#include <QObject>
#include "menupluginchooserinterface.h"

class QAction;
class QToolBar;

/**
 * @brief      Implementation for the MenuPluginChooserPlugin
 * @ingroup    src
 */
class MenuPluginChooser : public QObject, public MenuPluginChooserInterface {
  Q_OBJECT

  public:
    explicit MenuPluginChooser(LibFramework::PluginManagerInterface* pluginmanager_interface);

    void generatePluginMenu(QMenu* target, const QString& menu_name, const QString& plugin_namespace, CallbackFunction callback, const QKeySequence& shortcut, const QIcon& icon) const override;
    void generatePluginMenu(QMenu* target, const QString& menu_name, const QString& plugin_namespace, CallbackFunction callback) const override;

    void generatePluginToolbarItem(QToolBar* target, const QString& action_name, const QIcon& icon, const QString& plugin_namespace, CallbackFunction callback) const override;

  private:
    QAction* createPluginAction(QMenu* target, const QString& menu_name, const QString& plugin_namespace, CallbackFunction callback, const QKeySequence& shortcut = QKeySequence()) const;

    LibFramework::PluginManagerInterface *pluginmanager_interface;
};
