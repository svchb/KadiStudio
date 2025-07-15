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

#include <memory>

#include "../processmanagerinterface.h"


/**
 * @brief      Provides access to the widget factory of the opengl
 *             library. The plugin registers the widgets at the
 *             factory, so that it can produce qt widgets.
 * @ingroup    processmanager
 */
class ProcessManager : public ProcessManagerInterface {

  public:
    explicit ProcessManager(LibFramework::PluginManagerInterface* pluginmanager_interface);
    std::unique_ptr<WorkflowInterface> startWorkflow(const QString& workflowFile) override;
    void continueWorkflow(unsigned int workflowId) override;
    void cancelWorkflow(unsigned int workflowId) override;
    std::unique_ptr<WorkflowInterface> retrieveWorkflow(unsigned int workflowId) override;
    void inputValue(unsigned int workflowId, const QString& interactionId, const QString& value) override;
    std::vector<std::unique_ptr<WorkflowInterface>> retrieveWorkflows() override;
    std::vector<std::unique_ptr<WorkflowShortcut>> retrieveShortcuts(unsigned int workflowId) override;
    std::vector<std::unique_ptr<InteractionInterface>> retrieveInteractions(unsigned int workflowId) override;
    QString retrieveWorkflowLog(unsigned int workflowId) override;
    QString retrieveWorkflowLogPath(unsigned int workflowId) override;
    QString retrieveWorkflowTreePath(unsigned int workflowId) override;
    QJsonObject retrieveWorkflowTree(unsigned int workflowId) override;

  private:
    std::unique_ptr<WorkflowInterface> parseWorkflow(const QJsonObject& jsonWorkflowObject) const;
    static ShellResult readFromShell(const QString& command, const QStringList &arguments = {});

    const QString process_manager = "process-manager";

    WorkflowInterface *workflow_interface;
    InteractionInterface *interaction_interface;
};
