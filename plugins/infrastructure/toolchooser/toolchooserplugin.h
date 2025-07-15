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

class ToolChooser;
class ToolChooserInterface;


/**
 * @brief      A plugin which provides a widget to select a tool from the list
 *             of available tools (.kadistudio/tools.txt).
 * @ingroup    toolchooser
 */
class ToolChooserPlugin : public LibFramework::PluginInterface {

  public:
    void run() override;
    void load() override;
    void unload() override;

    LibFramework::InterfaceContainer* createInterfaces() override;

  private:
    ToolChooserInterface *interface;
};

PLUGIN_INSTANCE(ToolChooserPlugin)
PLUGIN_AUTHORS(
Markus Maier,
Hensgen O
)
PLUGIN_NAME(ToolChooser)
PLUGIN_DESCRIPTION(A widget to select a tool from the tools list)
PLUGIN_NAMESPACE(/plugins/infrastructure/toolchooser)
PLUGIN_ICON(:/studio/framework/application/pixmaps/noicon.png)
PLUGIN_REQUIRED_NAMESPACES(
  /plugins/infrastructure/kadiintegration
  /plugins/infrastructure/dialogs/registertooldialog
)
