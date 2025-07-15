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

#include "WorkflowExecutionDelegate.h"

class WorkflowExecution;

/**
 * @brief      Entry point for the workflowexecution plugin.
 * @ingroup    workflowexecution
 */
class WorkflowExecutionPlugin : public LibFramework::PluginInterface {

  public:
    void run() override;
    void load() override;
    void unload() override;

  LibFramework::InterfaceContainer* createInterfaces() override;

private:
  // ownership of the following pointers will be given away
  WorkflowExecutionDelegate *delegate{};
  WorkflowExecution *workflowExecution{};

};

PLUGIN_INSTANCE(WorkflowExecutionPlugin)
PLUGIN_AUTHORS(
  Philipp Zschumme
)
PLUGIN_NAME(WorkflowExecution)
PLUGIN_DESCRIPTION(Manage workflow execution)
PLUGIN_ICON(:/studio/framework/application/pixmaps/netzwerk.png)
PLUGIN_NAMESPACE(/plugins/application/workflowexecution)
PLUGIN_REQUIRED_NAMESPACES(
  /plugins/infrastructure/workflows/processmanager
  /plugins/infrastructure/dialogs/logdialog
  /plugins/infrastructure/menupluginchooser
  /plugins/infrastructure/workflows/processmanager/workflow
  /plugins/infrastructure/workflows/processmanager/interaction
)
