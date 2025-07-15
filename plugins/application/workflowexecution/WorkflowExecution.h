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
#include <QtWidgets/QWidget>
#include <QtWidgets/QListView>
#include <QtWidgets/QTableView>
#include <QtCore/QFileInfo>

#include <framework/pluginframework/pluginmanagerinterface.h>
#include <framework/statusbar/statusbardelegate.h>
#include <plugins/infrastructure/workflows/processmanager/workflow/workflowshortcut.h>
#include <plugins/infrastructure/workflows/processmanager/interaction/interactioninterface.h>

#include "workflowtable/WorkflowTableModel.h"
#include "workflowtable/WorkflowTableSortFilter.h"

class LogDialogInterface;
class ProcessManagerInterface;
class FileOpenDialogInterface;


/**
 * @brief      GUI to manage workflow execution.
 * @ingroup    workflowexecution
 */
class WorkflowExecution : public QWidget {
  Q_OBJECT

public:
  explicit WorkflowExecution(StatusBarInterface* statusBarInterface,
                             LibFramework::PluginManagerInterface* pluginmanager,
                             QWidget* parent = nullptr);

  void executeWorkflow(const QString& filepath);

  QMenu *getMenu();

  static void openExternally(const QString& path);
  std::vector<std::unique_ptr<WorkflowShortcut>> retrieveShortcuts(unsigned int workflowId);
  std::vector<std::unique_ptr<InteractionInterface>> retrieveInteractions(unsigned int workflowId);
  void showWorkflowLogDialog(unsigned int workflowId);

public Q_SLOTS:
  void refreshWorkflows();
  void openContextMenu(QPoint position);

private Q_SLOTS:
  void openLoadDialog(const QString& dialog_namespace);

private:
  LibFramework::PluginManagerInterface *pluginmanager;
  LogDialogInterface *logdialog_interface;
  ProcessManagerInterface *processmanager_interface;
  StatusBarInterface *statusBarInterface;
  WorkflowTableModel *tableModel;
  WorkflowTableSortFilter *tableModelProxy;
  QTimer *workflowRefreshTimer;
  QMenu *menu;
  QMenu *createMenu();
  const WorkflowInterface *selectedWorkflow;
  QTableView *workflowTable;

  void executeWorkflow(FileOpenDialogInterface *matchingFileDialogInterface, const QString& filepath);

  void switchToInteractionPlugin(const WorkflowInterface* workflow);
  void retrieveWorkflows();
  const WorkflowInterface* getWorkflowAtPosition(const QPoint& position) const;

  const static int REFRESH_INTERVAL_ACTIVE = 500;
  const static int REFRESH_INTERVAL_INACTIVE = 2000;
};
