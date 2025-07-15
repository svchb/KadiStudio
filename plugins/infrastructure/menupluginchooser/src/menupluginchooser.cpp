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
#include <QAction>
#include <QMenu>
#include <QToolBar>
#include <QToolButton>
#include <framework/pluginframework/pluginmanager.h>

#include "shortcutmenu.h"
#include "menupluginchooser.h"

MenuPluginChooser::MenuPluginChooser(LibFramework::PluginManagerInterface* pluginmanager_interface)
    : pluginmanager_interface(pluginmanager_interface) {

}

void MenuPluginChooser::generatePluginMenu(QMenu* target, const QString& menu_name, const QString& plugin_namespace, CallbackFunction callback, const QKeySequence& shortcut, const QIcon& icon) const {
  auto available_interfaces = pluginmanager_interface->getInterfaces(plugin_namespace.toStdString());

  if (available_interfaces.empty()) {
    auto placeholder_action = new QAction(menu_name);
    placeholder_action->setEnabled(false);
    placeholder_action->setShortcut(shortcut);
    placeholder_action->setToolTip(tr("Required namespace '%1' contains no plugins").arg(plugin_namespace));
    placeholder_action->setIcon(icon);
    target->addAction(placeholder_action);
  } else if (available_interfaces.size() == 1) {
    QAction *result = createPluginAction(target, menu_name, QString::fromStdString(available_interfaces.begin()->first), callback);
    result->setIcon(icon);
  } else {
    ShortcutMenu *sub_menu = new ShortcutMenu(menu_name, shortcut);
    sub_menu->setToolTipsVisible(true);
    QAction *action = nullptr;
    for (const auto& interface_entry : available_interfaces) {
      QString action_name, tooltip;
      QString sub_namespace = QString::fromStdString(interface_entry.first);
      auto plugin_infos = pluginmanager_interface->getPluginInfos(sub_namespace.toStdString());
      if (!plugin_infos.empty()) {
        action_name = QString::fromStdString(plugin_infos[0]->getName()); // assuming first element is the right one
        tooltip = QString::fromStdString(plugin_infos[0]->getDescription());
      } else {
        action_name = sub_namespace.split('/').last();
      }

      action = createPluginAction(sub_menu, action_name, sub_namespace, callback);
      action->setToolTip(tooltip);
    }
    sub_menu->setDefaultActionWithShortcut(action);
    sub_menu->setIcon(icon);
    target->addMenu(sub_menu);
  }
}

void MenuPluginChooser::generatePluginMenu(QMenu* target, const QString& menu_name, const QString& plugin_namespace, CallbackFunction callback) const {
  generatePluginMenu(target, menu_name, plugin_namespace, callback, QKeySequence(), QIcon());
}

void MenuPluginChooser::generatePluginToolbarItem(QToolBar* target, const QString& name, const QIcon& icon, const QString& plugin_namespace, CallbackFunction callback) const {
  auto available_interfaces = pluginmanager_interface->getInterfaces(plugin_namespace.toStdString());

  if (available_interfaces.empty()) {
    auto placeholder_action = new QAction(name);
    placeholder_action->setEnabled(false);
    placeholder_action->setToolTip(tr("Required namespace '%1' contains no plugins").arg(plugin_namespace));
    placeholder_action->setIcon(icon);
    target->addAction(placeholder_action);
  } else if (available_interfaces.size() == 1) {
    auto action = createPluginAction(nullptr, name, QString::fromStdString(available_interfaces.begin()->first), callback);
    action->setIcon(icon);
    target->addAction(action);
  } else {
    auto sub_menu = new QMenu();
    for (const auto &interface_entry: available_interfaces) {
      QString action_name, tooltip;
      QString sub_namespace = QString::fromStdString(interface_entry.first);
      auto plugin_infos = pluginmanager_interface->getPluginInfos(sub_namespace.toStdString());
      if (!plugin_infos.empty()) {
        action_name = QString::fromStdString(plugin_infos[0]->getName()); // assuming first element is the right one
        tooltip = QString::fromStdString(plugin_infos[0]->getDescription());
      } else {
        action_name = sub_namespace.split('/').last();
      }

      QAction *action = createPluginAction(sub_menu, action_name, sub_namespace, callback);
      action->setToolTip(tooltip);
      sub_menu->addAction(action);
    }

    auto toolButton = new QToolButton();
    toolButton->setToolTip(name);
    toolButton->setMenu(sub_menu);
    toolButton->setPopupMode(QToolButton::InstantPopup);
    toolButton->setIcon(icon);
    target->addWidget(toolButton);
  }
}


QAction* MenuPluginChooser::createPluginAction(QMenu* target, const QString& menu_name, const QString& plugin_namespace,
                                           CallbackFunction callback,
                                           const QKeySequence& shortcut) const {
  QAction *action = new QAction(menu_name);
  action->setShortcut(shortcut);
  connect(action, &QAction::triggered, this, [plugin_namespace, callback]() {
    callback(plugin_namespace);
  });
  target->addAction(action);
  return action;
}
