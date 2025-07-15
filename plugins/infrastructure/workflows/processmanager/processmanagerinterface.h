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

#include <framework/pluginframework/pluginclientinterface.h>
#include <framework/pluginframework/pluginmanagerinterface.h>

#include <plugins/infrastructure/workflows/processmanager/workflow/workflowshortcut.h>
#include <plugins/infrastructure/workflows/processmanager/workflow/workflowinterface.h>
#include <plugins/infrastructure/workflows/processmanager/interaction/interactioninterface.h>

struct ShellResult {
  int exit_code = -1;
  QString stdout_result;
  QString stderr_result;
};

/**
 * @brief      Provides access to the widget factory of the opengl
 *             library. The plugin registers the widgets at the
 *             factory, so that it can produce qt widgets.
 * @ingroup    processmanager
 */
class ProcessManagerInterface : public LibFramework::PluginClientInterface {

  public:
    virtual std::unique_ptr<WorkflowInterface> startWorkflow(const QString& workflowFile) = 0;
    virtual void continueWorkflow(unsigned int workflowId) = 0;
    virtual void cancelWorkflow(unsigned int workflowId) = 0;
    virtual std::unique_ptr<WorkflowInterface> retrieveWorkflow(unsigned int workflowId) = 0;
    virtual void inputValue(unsigned int workflowId, const QString& interactionId, const QString& value) = 0;
    virtual std::vector<std::unique_ptr<WorkflowInterface>> retrieveWorkflows() = 0;
    virtual std::vector<std::unique_ptr<WorkflowShortcut>> retrieveShortcuts(unsigned int workflowId) = 0;
    virtual std::vector<std::unique_ptr<InteractionInterface>> retrieveInteractions(unsigned int workflowId) = 0;
    virtual QString retrieveWorkflowLog(unsigned int workflowId) = 0;
    virtual QString retrieveWorkflowLogPath(unsigned int workflowId) = 0;
    virtual QString retrieveWorkflowTreePath(unsigned int workflowId) = 0;
    virtual QJsonObject retrieveWorkflowTree(unsigned int workflowId) = 0;

};
