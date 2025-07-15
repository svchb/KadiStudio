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

#include <framework/pluginframework/pluginmanagerinterface.h>
#include <framework/tab/tabdelegate.h>
#include <framework/statusbar/statusbardelegate.h>
#include <framework/menu/menudelegate.h>

#include "WorkflowExecution.h"

#include "workflowexecutionplugin.h"

void WorkflowExecutionPlugin::run() {
  TabInterface *tab = TabDelegate::getInstance();
  tab->addTab("/plugins/application/workflowexecution", workflowExecution, "WorkflowExecution");
  tab->setTabName("/plugins/application/workflowexecution", "Workflow Execution");

  MenuInterface *menu = MenuDelegate::getInstance();
  menu->addMenu("/plugins/application/workflowexecution", workflowExecution->getMenu());
}

void WorkflowExecutionPlugin::load() {
  StatusBarInterface *bar = StatusBarDelegate::getInstance();
  workflowExecution = new WorkflowExecution(bar, getPluginManager(), nullptr);
  delegate = new WorkflowExecutionDelegate(workflowExecution);
}

void WorkflowExecutionPlugin::unload() {
  // delete workflowExecution; // this would cause a double free because Qt already deletes this QWidget subclass object
  delete delegate;
}

LibFramework::InterfaceContainer *WorkflowExecutionPlugin::createInterfaces() {
  return new LibFramework::InterfaceContainer(delegate);
}
