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

#include <framework/pluginframework/plugininterface.h>

class MenuPluginChooserInterface;

/**
 * @brief      A plugin which can automatically generate a QMenu or QAction for a plugin namespace.
 *             If there are multiple plugins available in given namespace, it builds a QMenu including
 *             a QAction for each plugin, otherwise it will build just a QAction.
 * @ingroup    menupluginchooser
 */
class MenuPluginChooserPlugin : public LibFramework::PluginInterface {

  public:
    void run() override;
    void load() override;
    void unload() override;

    LibFramework::InterfaceContainer* createInterfaces() override;

  private:
    MenuPluginChooserInterface *interface;

};

PLUGIN_INSTANCE(MenuPluginChooserPlugin)
PLUGIN_AUTHORS(Philipp Zschumme)
PLUGIN_NAME(MenuPluginChooser)
PLUGIN_DESCRIPTION(Generates QMenu or QAction for a plugin namespace)
PLUGIN_NAMESPACE(/plugins/infrastructure/menupluginchooser)
PLUGIN_REQUIRED_NAMESPACES()
