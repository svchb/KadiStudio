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

#include <memory>

#include <QJsonDocument>
#include <QtCore/QJsonArray>
#include <QTimer>
#include <QUrl>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QInputDialog>
#include <QtWidgets/QMenu>
#include <QtConcurrent/QtConcurrentRun>
#include <QtGui/QDesktopServices>

#include <framework/tab/tabdelegate.h>
#include <framework/pluginframework/pluginchooser/pluginchooser.h>
#include <plugins/infrastructure/menupluginchooser/menupluginchooserinterface.h>
#include <plugins/infrastructure/dialogs/fileopen/fileopendialoginterface.h>
#include <plugins/application/workflowinteraction/WorkflowInteractionInterface.h>
#include <plugins/infrastructure/workflows/processmanager/workflow/workflowinterface.h>
#include <plugins/infrastructure/workflows/processmanager/processmanagerinterface.h>
#include <plugins/infrastructure/dialogs/logdialog/logdialoginterface.h>

#include "workflowtable/WorkflowTableModel.h"
#include "workflowtable/StatusDelegate.h"
#include "workflowtable/WorkflowTableSortFilter.h"

#include "WorkflowExecution.h"

WorkflowExecution::WorkflowExecution(StatusBarInterface *statusBarInterface,
                                     LibFramework::PluginManagerInterface *pluginmanager,
                                     QWidget *parent)
    : QWidget(parent), pluginmanager(pluginmanager), statusBarInterface(statusBarInterface),
      selectedWorkflow(nullptr), workflowTable(nullptr) {
  menu = createMenu();
  // note: if the parent is set, it gets ownership and QT takes care of deleting
  auto *layout = new QVBoxLayout(this);
  workflowTable = new QTableView(this);
  tableModel = new WorkflowTableModel(this);

  //retrieveWorkflows();

  tableModelProxy = new WorkflowTableSortFilter(this);
  tableModelProxy->setDynamicSortFilter(true);
  tableModelProxy->setSourceModel(tableModel);
  workflowTable->setModel(tableModelProxy);

  // wire/enable the context menu
  workflowTable->setContextMenuPolicy(Qt::CustomContextMenu);
  connect(workflowTable, &QTableView::customContextMenuRequested, this,
    &WorkflowExecution::openContextMenu);

  // table settings
  workflowTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch); // make cells stretch
  workflowTable->setEditTriggers(QAbstractItemView::NoEditTriggers); // disallow editing
  workflowTable->setSelectionBehavior(QAbstractItemView::SelectRows); // selection by row
  workflowTable->setSelectionMode(QAbstractItemView::NoSelection); // no selection
  workflowTable->setFocusPolicy(Qt::FocusPolicy::NoFocus);
  workflowTable->setAlternatingRowColors(true);
  workflowTable->setSortingEnabled(true);
  workflowTable->sortByColumn(3, Qt::DescendingOrder);

  // set custom delegate for the column with progressbar
  workflowTable->setItemDelegateForColumn(2, new StatusDelegate());

  layout->addWidget(workflowTable);
  QWidget::setLayout(layout);

  workflowRefreshTimer = new QTimer(this);
  workflowRefreshTimer->setInterval(REFRESH_INTERVAL_ACTIVE);
  connect(workflowRefreshTimer, &QTimer::timeout, this, &WorkflowExecution::refreshWorkflows);
  workflowRefreshTimer->start();

  qRegisterMetaType<QVector<int> >("QVector<int>");
  connect(tableModel, &WorkflowTableModel::dataChanged, this,
          [&](const QModelIndex &topLeft, const QModelIndex &bottomRight, const QVector<int> &) {

      /* This lambda is triggered whenever the data on the table model changes. It updates the affected rows */
      auto proxyIndex = tableModelProxy->mapFromSource(topLeft); // get the correct index (apply sorting)
      for (int i = 0; i < tableModel->columnCount(proxyIndex); i++) { // update all columns
        const QModelIndex &index = proxyIndex.sibling(proxyIndex.row(), i);
        workflowTable->update(index);
      }
      // assume that only one row has to be updated (this should not be triggered)
      if (bottomRight.row() != topLeft.row()) {
        qDebug() << "Warning: request to update more than one table rows is ignored";
      }
    });
  logdialog_interface = pluginmanager->getInterface<LogDialogInterface*>("/plugins/infrastructure/dialogs/logdialog");
  assert(logdialog_interface);
  processmanager_interface = pluginmanager->getInterface<ProcessManagerInterface*>("/plugins/infrastructure/workflows/processmanager");
  assert(processmanager_interface);
}

void WorkflowExecution::retrieveWorkflows() {
  if (!tableModel) return;
  try {
    std::vector<std::unique_ptr<WorkflowInterface>> workflows = processmanager_interface->retrieveWorkflows();
    for (auto& workflow : workflows) {
      if (workflow) {
        tableModel->addWorkflow(std::move(workflow));
      }
    }
  } catch(std::runtime_error& runtime_error) {
    statusBarInterface->showMessage("workflowExecution", "Retrieving the workflows failed");
    qDebug() << runtime_error.what();
  } catch(std::logic_error& logic_error) {
    statusBarInterface->showMessage("workflowExecution", "Parsing the workflows failed");
    qDebug() << logic_error.what();
  }
}

QMenu *WorkflowExecution::createMenu() {
  auto workflowMenu = new QMenu("Workflow");

  auto menu_plugin_chooser = pluginmanager->getInterface<MenuPluginChooserInterface*>("/plugins/infrastructure/menupluginchooser");
  menu_plugin_chooser->generatePluginMenu(workflowMenu, "&Open", "/plugins/infrastructure/dialogs/fileopen",
                                          [this](const QString &plugin_namespace) {
    openLoadDialog(plugin_namespace);
  }, QKeySequence("Ctrl+O"), QIcon(":/studio/plugins/application/workfloweditor/icons/fa-folder-open.svg")); // TODO Icon verschieben

  return workflowMenu;
}

void WorkflowExecution::refreshWorkflows()
{
  TabInterface *tab = TabDelegate::getInstance();
  if (tab->isTabActive(QStringLiteral("/plugins/application/workflowexecution"))) {
    QFuture<void> fut = QtConcurrent::run(&WorkflowExecution::retrieveWorkflows, this);

    workflowRefreshTimer->setInterval(REFRESH_INTERVAL_ACTIVE);
  } else {
    workflowRefreshTimer->setInterval(REFRESH_INTERVAL_INACTIVE);
  }

  // restart the timer
  workflowRefreshTimer->start();
}

void WorkflowExecution::openContextMenu(QPoint position) {
  auto workflowAtPosition = getWorkflowAtPosition(position);

  if (!workflowAtPosition) {
    return;
  }
  selectedWorkflow = workflowAtPosition;

  auto workflowId = selectedWorkflow->getId();
  auto shortcuts = retrieveShortcuts(workflowId);
  auto interactions = retrieveInteractions(workflowId);

  auto *contextMenu = new QMenu(this);
  auto showLogAction = new QAction(tr("Show Log"));
  contextMenu->addAction(showLogAction);
  connect(showLogAction, &QAction::triggered, this, [&] {
    showWorkflowLogDialog(selectedWorkflow->getId());
  });

  if (!interactions.empty()) {
    auto interactAction = new QAction(tr("Interact (%1)").arg(QString::number(interactions.size())));
    contextMenu->addAction(interactAction);
    connect(interactAction, &QAction::triggered, this, [this] { switchToInteractionPlugin(selectedWorkflow); });
  }

  WorkflowState state = selectedWorkflow->getState();
  if ((state == RUNNING || state == NEEDS_INTERACTION) && processmanager_interface) {
    auto cancel_action = new QAction(tr("Cancel execution"));
    connect(cancel_action, &QAction::triggered, this, [this] {
      try {
        processmanager_interface->cancelWorkflow(selectedWorkflow->getId());
      } catch (const std::exception& exception) {
        QMessageBox::critical(this, tr("Error"), tr("Error: Failed to cancel workflow execution!") + "\n" + exception.what());
      }
    });
    contextMenu->addAction(cancel_action);
  }

  //contextMenu->addSeparator();
  contextMenu->addSection(tr("Shortcuts"));
  for (const auto &shortcut : shortcuts) {
    auto shortcutAction = new QAction(shortcut->name);
    contextMenu->addAction(shortcutAction);
    auto path = shortcut->path;
    connect(shortcutAction, &QAction::triggered, this, [&] { openExternally(path); });
  }
  contextMenu->popup(workflowTable->viewport()->mapToGlobal(position));
}

const WorkflowInterface* WorkflowExecution::getWorkflowAtPosition(const QPoint& position) const {
  // the model proxy maps row numbers from the source model to actual row numbers in the table (as displayed)
  // the real row number of the model can be retrieved with QSortFilterProxyModel::mapToSource
  const int proxyRow = workflowTable->verticalHeader()->logicalIndexAt(position);
  const int row = tableModelProxy->mapToSource(tableModelProxy->index(proxyRow, 0)).row();

  return tableModel->getWorkflowAtRow(row);;
}

void WorkflowExecution::showWorkflowLogDialog(unsigned int workflowId) {
  logdialog_interface->showLogDialog(workflowId);
}

std::vector<std::unique_ptr<WorkflowShortcut>> WorkflowExecution::retrieveShortcuts(unsigned int workflowId) {
  try {
    return processmanager_interface->retrieveShortcuts(workflowId);
  } catch (std::exception& exception) {
    statusBarInterface->showMessage("workflowExecution", tr("Unable to retrieve workflow shortcuts for id %1").arg(QString::number(workflowId)));
  }
  return {};
}

std::vector<std::unique_ptr<InteractionInterface>> WorkflowExecution::retrieveInteractions(unsigned int workflowId) {
  try {
    std::vector<std::unique_ptr<InteractionInterface>> interactions =
      processmanager_interface->retrieveInteractions(workflowId);
    return interactions;
  } catch (std::exception& exception) {
    statusBarInterface->showMessage("workflowExecution", tr("Unable to retrieve interactions for id %1").arg(QString::number(workflowId)));
  }
  return {};
}

void WorkflowExecution::openExternally(const QString &path) {
  QDesktopServices::openUrl(QUrl::fromLocalFile(path));
}

void WorkflowExecution::switchToInteractionPlugin(const WorkflowInterface *workflow) {

  QString interactionPluginNamespace = "/plugins/application/workflowinteraction";

  // switch to workflow execution plugin
  if (!pluginmanager->isRunning(interactionPluginNamespace.toStdString())) {
    pluginmanager->run(interactionPluginNamespace.toStdString());
  } else {
    TabInterface *tab = TabDelegate::getInstance();
    tab->setActiveTab(interactionPluginNamespace);
  }

  // set workflow in interaction plugin
  auto *workflowInteractionInterface =
    pluginmanager->getInterface<WorkflowInteractionInterface*>(interactionPluginNamespace.toStdString());
  if (workflowInteractionInterface) {
    workflowInteractionInterface->setWorkflowId(workflow->getId());
  } else {
    QMessageBox::critical(this, tr("Error"), tr("Can not switch to the workflow interaction plugin, is it installed and loaded?\n"
                                                "This can also be caused by a problem when loading the plugin's library."));
    qDebug() << "Error: Could not get interface of plugin workflowinteraction";
  }
}

void WorkflowExecution::executeWorkflow(FileOpenDialogInterface* matchingFileDialogInterface, const QString& filepath) {
  if (matchingFileDialogInterface && matchingFileDialogInterface->validateAndLoadFilePath(filepath)) {
    try {
      tableModel->addWorkflow(processmanager_interface->startWorkflow(matchingFileDialogInterface->getCachedFilePath()));
      refreshWorkflows();
    } catch(std::exception& exception) {
      QMessageBox::critical(this, tr("Error"), tr("The workflow could not be started") + "\n" + exception.what());
    }
  } else {
    QMessageBox::critical(this, tr("Error"), tr("Unable to open the selected workflow file '%1'").arg(filepath));
  }
}

void WorkflowExecution::executeWorkflow(const QString& filepath) {
  FileOpenDialogInterface *matchingFileDialogInterface = FileOpenDialogInterface::getCompatibleFileOpenPlugin(pluginmanager, filepath);
  executeWorkflow(matchingFileDialogInterface, filepath);
}

void WorkflowExecution::openLoadDialog(const QString& dialog_namespace) {
  PluginChooser pluginchooser(this);
  auto fileDialog = pluginchooser.chooseFrom<FileOpenDialogInterface *>(pluginmanager, dialog_namespace);
  if (!fileDialog) {
    return;
  }
  fileDialog->setFileMode(FileOpenDialogInterface::ExistingFile);
  fileDialog->applyFilter(tr("Workflow Files (*.flow)"));
  if (fileDialog->showFileOpenDialog()) {
    executeWorkflow(fileDialog, fileDialog->getFilePath());
  }
}

QMenu *WorkflowExecution::getMenu() {
  return menu;
}
