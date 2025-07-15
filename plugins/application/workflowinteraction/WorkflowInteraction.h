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

#include <QWidget>
#include <QString>

class QTimer;
class QMenu;
class QPushButton;
class StatusBarInterface;

#include <framework/pluginframework/pluginmanagerinterface.h>
#include <plugins/infrastructure/workflows/processmanager/workflow/workflowinterface.h>
#include <plugins/infrastructure/workflows/processmanager/workflow/workflowshortcut.h>
#include <plugins/infrastructure/workflows/processmanager/interaction/interactioninterface.h>

#include "widgets/WorkflowInfoWidget.h"

class InteractionWidget;
class ProcessManagerInterface;
class RecentFiles;

class WorkflowInteraction  : public QWidget {
  Q_OBJECT

public:
  explicit WorkflowInteraction(LibFramework::PluginManagerInterface *pluginmanager, StatusBarInterface *statusBarInterface,
                               QWidget *parent = nullptr);

  void dragEnterEvent(QDragEnterEvent* e) Q_DECL_OVERRIDE;
  void dragMoveEvent(QDragMoveEvent* e) Q_DECL_OVERRIDE;
  void dropEvent(QDropEvent* e) Q_DECL_OVERRIDE;

  QMenu *getMenu();

  /**
   * Initialize a workflow which is already registered with the process manager and therefore has an id.
   * The workflow info (file name, status etc.) can be retrieved from the process manager using the id
   * @param id of the workflow
   */
  void initializeWorkflow(unsigned int id);

  /**
   * Initialize a workflow which is not yet registered with the process manager and therefore has no id.
   * This means the workflow has not been pushed to the process manager for execution and is in the state READY.
   * Instead of retrieving the workflow info from the process manager API, create a new Workflow instance.
   *
   * Also the periodic refreshing of workflow information is stopped when this method is called.
   *
   * @param fileName File name of the workflow definition file
   */
  void initializeWorkflow(const QString& fileName);

Q_SIGNALS:
  void workflowLoaded(bool loaded);
  void shortcutsAvailable(bool value);
  void disableWidgets(bool disabled);

private Q_SLOTS:
  void onStartContinueButtonPressed();
  void onCancelButtonPressed();
  void refreshWorkflow();
  void switchToExecutionTab();
  void openLoadDialog(const QString& dialog_namespace);

private:
  void createMenu();
  std::vector<std::unique_ptr<InteractionInterface>> loadInteractions();
  void updateInteractionWidgets();
  bool performContinueWorkflow();
  bool performStartWorkflow();
  bool performCancelWorkflow();
  std::unique_ptr<WorkflowInterface> retrieveWorkflow(unsigned int workflowId);
  void updateButtonsForCurrentState();
  void clearShortcutsMenu();
  void generateShortcutsMenu();
  void setupPrimaryButtonStyle(QPushButton* button);
  void setupSecondaryButtonStyle(QPushButton* button);
  void setupDangerButtonStyle(QPushButton* button);

  // interface to other plugins
  LibFramework::PluginManagerInterface *pluginmanager;
  StatusBarInterface *statusBarInterface;
  ProcessManagerInterface *processmanager_interface;

  // data related
  bool workflowInitialized;
  std::unique_ptr<WorkflowInterface> workflow;
  std::vector<std::unique_ptr<WorkflowShortcut>> shortcuts;
  std::vector<std::unique_ptr<InteractionInterface>> interactions;
  QTimer *workflowRefreshTimer;
  bool forceWidgetRefresh;

  // user interface
  QMenu *menu;
  QPushButton *startContinueButton;
  QPushButton *cancel_button;
  WorkflowInfoWidget *workflowInfoWidget;
  QMenu *shortcutsMenu;
  std::vector<QAction*> shortcutActions;
  InteractionWidget *interactionWidget;

  RecentFiles *recentfiles;

};
