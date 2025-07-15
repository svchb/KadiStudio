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
#include <QtCore/QProcess>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QJsonArray>

#include "processmanager.h"

ProcessManager::ProcessManager(LibFramework::PluginManagerInterface* pluginmanager_interface) {
  workflow_interface = pluginmanager_interface->getInterface<WorkflowInterface*>("/plugins/infrastructure/workflows/processmanager/workflow");
  assert(workflow_interface);
  interaction_interface = pluginmanager_interface->getInterface<InteractionInterface*>("/plugins/infrastructure/workflows/processmanager/interaction");
  assert(workflow_interface);
}

std::unique_ptr<WorkflowInterface> ProcessManager::startWorkflow(const QString& workflowFile) {
  auto result = readFromShell(process_manager, {"start", "--no-color", workflowFile});

  if (result.exit_code == 0) {
    QString jsonString = result.stdout_result;
    QJsonDocument jsonDocument(QJsonDocument::fromJson(jsonString.toUtf8()));
    return parseWorkflow(jsonDocument.object());
  } else {
    throw std::runtime_error(result.stderr_result.toStdString());
  }
}

void ProcessManager::continueWorkflow(unsigned int workflowId) {
  auto result = readFromShell(process_manager, {"continue", QString::number(workflowId)});

  if (result.exit_code != 0) {
    throw std::runtime_error(result.stderr_result.toStdString());
  }
}

void ProcessManager::cancelWorkflow(unsigned int workflowId) {
  auto process = new QProcess();
  process->start(process_manager, {"cancel", QString::number(workflowId)});
}

void ProcessManager::inputValue(unsigned int workflowId, const QString& interactionId, const QString& value) {
  QStringList args = {"input", QString::number(workflowId), interactionId, "'" + value + "'"};
  auto inputResult = readFromShell(process_manager, args);
  if (inputResult.exit_code != 0) {
    throw std::runtime_error(inputResult.stderr_result.toStdString());
  }
}

std::unique_ptr<WorkflowInterface> ProcessManager::retrieveWorkflow(unsigned int workflowId) {
  auto processManagerResult = readFromShell(process_manager, {"status", QString::number(workflowId)});

  if (processManagerResult.exit_code != 0) {
    throw std::runtime_error(processManagerResult.stderr_result.toStdString());
  }
  QString processManagerOutput = processManagerResult.stdout_result;

  QJsonDocument jsonDocument(QJsonDocument::fromJson(processManagerOutput.toUtf8()));

  auto parsedWorkflow = workflow_interface->create();
  parsedWorkflow->fromJson(jsonDocument.object());
  return parsedWorkflow;
}

std::vector<std::unique_ptr<WorkflowInterface>> ProcessManager::retrieveWorkflows() {
  auto result = readFromShell(process_manager,  {"list", "workflows"});
  if (result.exit_code != 0) {
    throw std::runtime_error(result.stderr_result.toStdString());
  }
  std::vector<std::unique_ptr<WorkflowInterface>> workflows;

  QString processManagerOutput = result.stdout_result;

  QJsonDocument jsonDocument(QJsonDocument::fromJson(processManagerOutput.toUtf8()));
  QJsonArray jsonArray = jsonDocument.array();

  for (const auto& jsonWorkflowRef : jsonArray) {
    auto parsedWorkflow = parseWorkflow(jsonWorkflowRef.toObject());
    if (parsedWorkflow) {
      workflows.push_back(std::move(parsedWorkflow));
    }
  }
  return workflows;
}

std::vector<std::unique_ptr<WorkflowShortcut>> ProcessManager::retrieveShortcuts(unsigned int workflowId) {
  std::vector<std::unique_ptr<WorkflowShortcut>> shortcuts;
  auto shortcutsOutput = readFromShell(process_manager, {"shortcuts", QString::number(workflowId)});
  if (shortcutsOutput.exit_code != 0) {
    throw std::runtime_error(shortcutsOutput.stderr_result.toStdString());
  }
  QString jsonString = shortcutsOutput.stdout_result;
  if (!jsonString.isEmpty()) {
    QJsonDocument jsonDocument(QJsonDocument::fromJson(jsonString.toUtf8()));
    QJsonObject jsonRootObject = jsonDocument.object();
    if (!jsonRootObject["shortcuts"].isUndefined()) {
      QJsonArray jsonArray = jsonRootObject["shortcuts"].toArray();

      for (const auto &shortcut : jsonArray) {
        auto shortcutObject = shortcut.toObject();
        auto result = std::make_unique<WorkflowShortcut>();
        if (!shortcutObject["name"].isUndefined()) {
          result->name = shortcutObject["name"].toString();
        }
        if (!shortcutObject["path"].isUndefined()) {
          result->path = shortcutObject["path"].toString();
        }
        if (!(result->path.isEmpty() || result->name.isEmpty())) {
          shortcuts.push_back(std::move(result));
        }
      }
    }
  }
  return shortcuts;
}

std::vector<std::unique_ptr<InteractionInterface>> ProcessManager::retrieveInteractions(unsigned int workflowId) {
  auto shortcutsOutput = readFromShell(process_manager, {"interactions", QString::number(workflowId)});
  if (shortcutsOutput.exit_code != 0) {
    throw std::runtime_error(shortcutsOutput.stderr_result.toStdString());
  }

  std::vector<std::unique_ptr<InteractionInterface>> interactions;
  QString jsonString = shortcutsOutput.stdout_result;
  if (!jsonString.isEmpty()) {
    QJsonDocument jsonDocument(QJsonDocument::fromJson(jsonString.toUtf8()));
    QJsonObject jsonRootObject = jsonDocument.object();
    if (!jsonRootObject["interactions"].isUndefined()) {
      QJsonArray jsonArray = jsonRootObject["interactions"].toArray();

      for (const auto &interactionJsonRef : jsonArray) {
        auto interaction = interaction_interface->create();
        auto interactionObject = interactionJsonRef.toObject();
        interaction->fromJson(interactionObject);

        interactions.push_back(std::move(interaction));
      }
    }
  }
  return interactions;
}

QString ProcessManager::retrieveWorkflowLog(unsigned int workflowId) {
  auto result = readFromShell(process_manager, {"log", QString::number(workflowId)});
  if (result.exit_code != 0) {
    throw std::runtime_error(result.stderr_result.toStdString());
  }
  return result.stdout_result;
}

QString ProcessManager::retrieveWorkflowLogPath(unsigned int workflowId) {
  auto result = readFromShell(process_manager, {"log_path", QString::number(workflowId)});
  if (result.exit_code != 0) {
    throw std::runtime_error(result.stderr_result.toStdString());
  }
  return result.stdout_result.trimmed();
}

QString ProcessManager::retrieveWorkflowTreePath(unsigned int workflowId) {
  auto result = readFromShell(process_manager, {"tree_path", QString::number(workflowId)});
  if (result.exit_code != 0) {
    throw std::runtime_error(result.stderr_result.toStdString());
  }
  return result.stdout_result.trimmed();
}

QJsonObject ProcessManager::retrieveWorkflowTree(unsigned int workflowId) {
  QJsonObject result;
  ShellResult shell_result = readFromShell(process_manager, {"tree", QString::number(workflowId)});
  if (shell_result.exit_code != 0) {
    throw std::runtime_error(shell_result.stderr_result.toStdString());
  }
  QJsonDocument doc;
  doc = QJsonDocument::fromJson(shell_result.stdout_result.toUtf8());
  if (!doc.isNull() && doc.isObject()) {
    result = doc.object();
  }
  return result;
}

ShellResult ProcessManager::readFromShell(const QString& command, const QStringList &arguments) {
  ShellResult result;

  QProcess process;

  process.start(command, arguments);
  process.waitForFinished();

  result.exit_code = process.exitCode();


  result.stdout_result = QString(process.readAllStandardOutput());
  result.stderr_result = QString(process.readAllStandardError());
  process.close();

  return result;
}

std::unique_ptr<WorkflowInterface> ProcessManager::parseWorkflow(const QJsonObject& jsonWorkflowObject) const {
  auto workflow = workflow_interface->create();
  workflow->fromJson(jsonWorkflowObject);
  return workflow;
}
