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
#include <framework/menu/menudelegate.h>
#include <framework/statusbar/statusbardelegate.h>

#include "WorkflowInteractionDelegate.h"

#include "WorkflowInteractionPlugin.h"

void WorkflowInteractionPlugin::run() {
  TabInterface *tab = TabDelegate::getInstance();
  tab->addTab("/plugins/application/workflowinteraction", workflowInteraction, "WorkflowInteraction");
  tab->setTabName("/plugins/application/workflowinteraction", "Workflow interaction");

  MenuInterface *menu = MenuDelegate::getInstance();
  menu->addMenu("/plugins/application/workflowinteraction", workflowInteraction->getMenu());

  if (delegate->getWorkflowFileName()) {
    workflowInteraction->initializeWorkflow(delegate->getWorkflowFileName());
  }
}

void WorkflowInteractionPlugin::load() {
  LibFramework::PluginManagerInterface *pluginmanager = LibFramework::PluginInterface::getPluginManager();
  StatusBarInterface *bar = StatusBarDelegate::getInstance();
  workflowInteraction = new WorkflowInteraction(pluginmanager, bar, nullptr);
  delegate = new WorkflowInteractionDelegate(workflowInteraction);
}

void WorkflowInteractionPlugin::unload() {
  // delete workflowInteraction; // this would cause a double free because Qt already deletes this QWidget subclass object
  delete delegate;
}

LibFramework::InterfaceContainer *WorkflowInteractionPlugin::createInterfaces() {
  return new LibFramework::InterfaceContainer(delegate);
}
