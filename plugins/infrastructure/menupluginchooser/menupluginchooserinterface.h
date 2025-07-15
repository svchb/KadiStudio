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

#include <functional>
#include <QKeySequence>
#include <framework/pluginframework/plugininterface.h>

class QMenu;
class QToolBar;

class MenuPluginChooserInterface: public LibFramework::PluginClientInterface {

  public:
    MenuPluginChooserInterface() = default;
    ~MenuPluginChooserInterface() override = default;

    using CallbackFunction = const std::function<void(const QString&)>;
    /**
     * Generate a QAction for each plugin in given namespace and connect provided callback to it's trigger event.
     * If multiple QActions are generated, they will be nested in a submenu (QMenu).
     * The result (QAction or QMenu) will be added to given target (QMenu*) via addAction() or addMenu() respectively.
     *
     * The provided callback has to accept the (sub)namespace of the plugin as `const std::string&` and is called via the connect,
     * whenever the user triggers the QAction, with the respective sub(namespace).
     * The plugin namespace can be used to obtain the plugin interface of type T in the callback. Since this function can not
     * know the type T, the interface can not be provided to the callback function directly.
     *
     * The top element will always have the name from the parameter `menu_name`
     *
     * @param target Target QMenu* to add the generated result (QMenu or QAction) via addMenu() or addAction(). Must not be null!
     * @param menu_name Name of the generated QMenu (or QAction if there is only one plugin in the namespace).
     * @param plugin_namespace Namespace containing one or multiple plugins.
     * @param callback Callback responsible to perform the desired action when any of the generated QActions is triggered.
     */
    virtual void generatePluginMenu(QMenu* target, const QString& menu_name, const QString& plugin_namespace, CallbackFunction callback, const QKeySequence& shortcut, const QIcon& icon) const = 0;
    virtual void generatePluginMenu(QMenu* target, const QString& menu_name, const QString& plugin_namespace, CallbackFunction callback) const = 0;

    /**
     * Generate a toolbar item, which is a normal QAction if there are zero or one plugins available for the given namespace.
     * In case there are multiple available plugins, a QToolButton will be generated. The QToolButton allows to open a submenu containing the QActions
     * for each available plugin.
     * Shortcuts are not supported, since there can not be a "default" plugin.
     * A non-null pointer to the target QToolBar must be given as first parameter in order to add the result via QToolBar::addAction() for QAction,
     * and QToolBar::addWidget() for the QToolButton respectively.
     *
     * @param target Non-null pointer to the target QToolBar, to which the result will be added.
     * @param action_name Name / tooltip for the top-level QAction or QToolButton.
     * @param icon Icon for the top-level QAction or QToolButton.
     * @param plugin_namespace Namespace containing one or multiple plugins.
     * @param callback Callback responsible to perform the desired action when any of the generated QActions is triggered.
     */
    virtual void generatePluginToolbarItem(QToolBar* target, const QString& name, const QIcon& icon, const QString& plugin_namespace, CallbackFunction callback) const = 0;
};
