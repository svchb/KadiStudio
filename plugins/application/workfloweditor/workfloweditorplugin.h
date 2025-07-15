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

class Workfloweditor;

class WorkfloweditorPlugin : public LibFramework::PluginInterface {

  public:
    void run() override;
    void load() override;
    void unload() override;

    LibFramework::InterfaceContainer* createInterfaces() override;

  private:
    Workfloweditor *workfloweditor{};

};

PLUGIN_INSTANCE(WorkfloweditorPlugin)
PLUGIN_AUTHORS(Philipp Zschumme)
PLUGIN_NAME(Workfloweditor)
PLUGIN_DESCRIPTION(This plugin allows user to visually create/edit Kadi workflows)
PLUGIN_ICON(:/studio/plugins/application/workfloweditor/icons/flow-svgrepo-com.svg)
PLUGIN_HELP(:/studio/plugins/application/workfloweditor/help/help.txt)
PLUGIN_NAMESPACE(/plugins/application/workfloweditor)
PLUGIN_REQUIRED_NAMESPACES(
  /plugins/infrastructure/toolchooser
  /plugins/infrastructure/kadiintegration
  /plugins/infrastructure/menupluginchooser
)
