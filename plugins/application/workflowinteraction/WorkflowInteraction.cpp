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

#include <algorithm>
#include <memory>

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <QGroupBox>
#include <QMessageBox>
#include <QDesktopServices>
#include <QTimer>
#include <QDir>
#include <QDrag>
#include <QDragEnterEvent>
#include <QMimeData>
#include <QScrollArea>

#include <framework/enhanced/recentfiles.h>
#include <framework/tab/tabdelegate.h>
#include <framework/statusbar/statusbardelegate.h>

#include <framework/pluginframework/pluginmanagerinterface.h>
#include <framework/pluginframework/pluginchooser/pluginchooser.h>
#include <plugins/infrastructure/menupluginchooser/menupluginchooserinterface.h>
#include <plugins/infrastructure/dialogs/logdialog/logdialoginterface.h>
#include <plugins/infrastructure/dialogs/fileopen/fileopendialoginterface.h>
#include <plugins/infrastructure/workflows/processmanager/processmanagerinterface.h>
#include "plugins/infrastructure/workflows/processmanager/workflow/workflowstate.h"
#include "widgets/InteractionWidget.h"

#include "WorkflowInteraction.h"

WorkflowInteraction::WorkflowInteraction(LibFramework::PluginManagerInterface* pluginmanager, StatusBarInterface* statusBarInterface, QWidget* parent)
    : QWidget(parent), pluginmanager(pluginmanager), statusBarInterface(statusBarInterface),
      workflowInitialized(false), forceWidgetRefresh(false),
      interactionWidget(new InteractionWidget(pluginmanager, this)) {
  auto layout = new QHBoxLayout(this);
  setLayout(layout);
  createMenu();

  setAcceptDrops(true);

  processmanager_interface = pluginmanager->getInterface<ProcessManagerInterface*>("/plugins/infrastructure/workflows/processmanager");
  assert(processmanager_interface);

  connect(this, &WorkflowInteraction::disableWidgets, interactionWidget, &InteractionWidget::setWidgetsDisabled);

  // buttons
  auto buttonGroup = new QGroupBox();
  buttonGroup->setStyleSheet("QGroupBox { border: none; }");
  auto buttonGroupLayout = new QVBoxLayout();
  buttonGroupLayout->setAlignment(buttonGroup, Qt::AlignTop);
  buttonGroup->setLayout(buttonGroupLayout);

  // Start/Continue-Button (primary)
  startContinueButton = new QPushButton();
  setupPrimaryButtonStyle(startContinueButton);
  startContinueButton->setIcon(QIcon(":/studio/plugins/application/workflowinteraction/icons/play.svg"));
  updateButtonsForCurrentState();
  connect(startContinueButton, &QPushButton::clicked, this, &WorkflowInteraction::onStartContinueButtonPressed);
  buttonGroupLayout->addWidget(startContinueButton);

  // Results-Button (secondary)
  auto shortcutsButton = new QPushButton("Results");
  shortcutsButton->setIcon(QIcon(":/studio/plugins/application/workflowinteraction/icons/square-poll.svg"));
  shortcutsMenu = new QMenu();
  //shortcutsMenu->addAction(new QAction("Open folder"));
  //shortcutsMenu->addSeparator();
  generateShortcutsMenu();
  shortcutsButton->setMenu(shortcutsMenu);
  buttonGroupLayout->addWidget(shortcutsButton);
  connect(this, &WorkflowInteraction::shortcutsAvailable, shortcutsButton, [shortcutsButton](bool shortcutsAvailable) {
    shortcutsButton->setDisabled(!shortcutsAvailable);
  });
  emit shortcutsAvailable(!shortcuts.empty());

  // Log-Button (secondary)
  auto logButton = new QPushButton("Log");
  logButton->setIcon(QIcon(":/studio/plugins/application/workflowinteraction/icons/clipboard-list.svg"));
  buttonGroupLayout->addWidget(logButton);
  connect(this, &WorkflowInteraction::workflowLoaded, logButton, [logButton](bool initialized) {
    logButton->setDisabled(!initialized);
  });
  logButton->setDisabled(true);
  connect(logButton, &QPushButton::clicked, this, [this, pluginmanager] {
    if (workflow) {
      LogDialogInterface *logdialog = this->pluginmanager->getInterface<LogDialogInterface*>("/plugins/infrastructure/dialogs/logdialog");
      if (!logdialog) {
        QMessageBox::critical(this, tr("Error"), tr("Could not open the log dialog plugin"));
        return;
      }
      try {
        unsigned int workflowId = workflow->getId();
        logdialog->showLogDialog(workflowId);
      } catch (std::runtime_error& error) {
        QMessageBox::critical(this, tr("Error"), tr("Could not retrieve the log") + "\n" + error.what());
      }
    }
  });

  // Cancel-Button (danger)
  cancel_button = new QPushButton("Cancel");
  setupDangerButtonStyle(cancel_button);
  cancel_button->setIcon(QIcon(":/studio/plugins/application/workflowinteraction/icons/fa-ban.svg"));
  cancel_button->setToolTip("Cancel Execution");
  cancel_button->setEnabled(false);
  connect(cancel_button, &QPushButton::clicked, this, &WorkflowInteraction::onCancelButtonPressed);
  buttonGroupLayout->addWidget(cancel_button);

  // buttonGroup Style settings
  buttonGroupLayout->addStretch();

  // History-Button (secondary)
  auto historyButton = new QPushButton("History");
  historyButton->setIcon(QIcon(":/studio/plugins/application/workflowinteraction/icons/book.svg"));
  buttonGroupLayout->addWidget(historyButton);
  connect(historyButton, &QPushButton::clicked, this, &WorkflowInteraction::switchToExecutionTab);

  // applies the setupSecondaryButtonStyle method to all secondary buttons
  QList<QPushButton*> secondaryButtons = {shortcutsButton, historyButton, logButton};

  for (auto button : secondaryButtons) {
    setupSecondaryButtonStyle(button);
  }

  // workflow info with progress bar
  auto infoGroup = new QGroupBox();
  auto infoGroupLayout = new QHBoxLayout();
  infoGroup->setLayout(infoGroupLayout);
  workflowRefreshTimer = new QTimer(this);
  workflowRefreshTimer->setInterval(500);
  connect(workflowRefreshTimer, &QTimer::timeout, this, &WorkflowInteraction::refreshWorkflow,
          Qt::ConnectionType::QueuedConnection);
  workflowInfoWidget = new WorkflowInfoWidget(pluginmanager);
  infoGroupLayout->addWidget(workflowInfoWidget);

  // setting up the right Layout
  auto rightWidget = new QWidget();
  auto rightWidgetLayout = new QVBoxLayout();
  rightWidget->setLayout(rightWidgetLayout);
  rightWidgetLayout->addWidget(interactionWidget);
  rightWidgetLayout->addWidget(infoGroup);

  // workflowInteraction overall layout
  layout->addWidget(buttonGroup);
  layout->addWidget(rightWidget);
}

QMenu *WorkflowInteraction::getMenu() {
  return menu;
}

std::vector<std::unique_ptr<InteractionInterface>> WorkflowInteraction::loadInteractions() {
  if (!workflow) {
    statusBarInterface->showMessage("workflow interactions",
                                    tr("Error: no workflow available\n"));
    return {};
  }

  std::vector<std::unique_ptr<InteractionInterface>> updated_interactions;
  try {
    updated_interactions = processmanager_interface->retrieveInteractions(workflow->getId());
  } catch (const std::exception& exception) {
    QMessageBox::critical(this, tr("Error"),
                          tr("Unable to receive interactions info from process manager.") + "\n\n" + exception.what());
  }
  return updated_interactions;
}

void WorkflowInteraction::updateInteractionWidgets() {
  std::vector<InteractionInterface*> interactionPtrs;
  interactionPtrs.reserve(interactions.size());
  for (const auto& interaction : interactions) {
    interactionPtrs.push_back(interaction.get());
  }

  try {
    interactionWidget->addInteractions(std::move(interactionPtrs));
  } catch (const std::exception& exception) {
    QMessageBox::critical(this, tr("Error"), tr("Unable to generate interactions.") + "\n\n" + exception.what());
  }
  interactionWidget->updateView();
}


void WorkflowInteraction::initializeWorkflow(unsigned int id) {
  if (workflowInitialized) {
    interactionWidget->reset();
    clearShortcutsMenu();
  }
  workflow = retrieveWorkflow(id);
  emit workflowLoaded(workflow != nullptr);
  workflowInfoWidget->setWorkflow(workflow.get());
  workflowInitialized = true;

  interactionWidget->setWorkflowId(id);
  interactions = std::move(loadInteractions());
  if (interactions.size() > 0) {
    updateInteractionWidgets();
    if (workflow && workflow->getState() == NEEDS_INTERACTION) {
      startContinueButton->setEnabled(true);
    }
  }
  interactionWidget->updateView();
  workflowInfoWidget->updateInfo();

  try {
    shortcuts = std::move(processmanager_interface->retrieveShortcuts(id));
    emit shortcutsAvailable(!shortcuts.empty());
    generateShortcutsMenu();
  } catch (const std::exception& exception) {
    statusBarInterface->showMessage("workflow interactions",
                                    tr("Unable to receive shortcuts info from process manager"));
  }

  updateButtonsForCurrentState();
  workflowRefreshTimer->start();
}

void WorkflowInteraction::initializeWorkflow(const QString& filePath) {
  TabInterface *tab = TabDelegate::getInstance();

  // no periodic refreshing necessary until the workflow is started
  workflowRefreshTimer->stop();

  interactionWidget->updateView();

  if (workflowInitialized) {
    interactionWidget->reset();
    clearShortcutsMenu();
    shortcuts.clear();
    interactions.clear();
    emit shortcutsAvailable(false);
  }
  auto workflow_interface = pluginmanager->getInterface<WorkflowInterface*>("/plugins/infrastructure/workflows/processmanager/workflow");
  assert(workflow_interface);
  workflow = workflow_interface->create();
  workflow->setFileName(filePath);
  workflowInitialized = true;

  emit workflowLoaded(false);

  // this workflow has no id yet, so we can not retrieve any info from the process_manager
  workflowInfoWidget->setWorkflow(workflow.get());
  workflowInfoWidget->updateInfo();
  updateButtonsForCurrentState(); // to make the start button available
  tab->setTabName("/plugins/application/workflowinteraction", QString("Workflow interaction") + " - " + filePath);
}

void WorkflowInteraction::openLoadDialog(const QString& dialog_namespace) {
  PluginChooser pluginchooser(this);
  auto fileDialog = pluginchooser.chooseFrom<FileOpenDialogInterface *>(pluginmanager, dialog_namespace);
  if (!fileDialog) {
    return;
  }
  fileDialog->setFileMode(FileOpenDialogInterface::ExistingFile);
  fileDialog->applyFilter(tr("Workflow Files (*.flow)"));
  if (fileDialog->showFileOpenDialog()) {
    initializeWorkflow(fileDialog->getFilePath());
  }
}

void WorkflowInteraction::onStartContinueButtonPressed() {
  if (!workflow) return;
  bool assumeRunning = false;
  startContinueButton->setFocus();
  emit workflowLoaded(true);
  switch (workflow->getState()) {
    case NEEDS_INTERACTION:
      assumeRunning = performContinueWorkflow();
      forceWidgetRefresh = true;
      break;
    case READY:
      assumeRunning = performStartWorkflow();
      interactionWidget->setWorkflowId(workflow->getId());
      break;
    default:
      break;
  }
  if (assumeRunning) {
    startContinueButton->setDisabled(true); // to avoid spamming of the button
    // the button will be enabled again if necessary in refreshWorkflow()

    workflowRefreshTimer->start(); // start to periodically refresh since execution is in progress
  }
}

void WorkflowInteraction::onCancelButtonPressed() {
  startContinueButton->setDisabled(true);
  emit disableWidgets(true);
  performCancelWorkflow();
}

std::unique_ptr<WorkflowInterface> WorkflowInteraction::retrieveWorkflow(unsigned int workflowId) {
  try {
    return processmanager_interface->retrieveWorkflow(workflowId);
  } catch (const std::exception& exception) {
    statusBarInterface->showMessage("workflow interactions",
                                    tr("Retrieving the workflow failed") + "\n" + exception.what());
    qDebug() << exception.what();
  }
  return {};
}

void WorkflowInteraction::refreshWorkflow() {
  auto *tab = TabDelegate::getInstance();
  if (tab->isTabActive("/plugins/application/workflowinteraction") && workflow) {
    auto updatedWorkflow = retrieveWorkflow(workflow->getId());

    if (!updatedWorkflow) {
      // refreshing failed -> do nothing, keep old workflow info
      return;
    }

    if (*updatedWorkflow != *workflow) {
      workflow = std::move(updatedWorkflow);
      workflowInfoWidget->setWorkflow(workflow.get());
      workflowInfoWidget->updateInfo();
    }

    if (workflow->getState() == NEEDS_INTERACTION) {
      auto updated_interactions = loadInteractions();
      if (forceWidgetRefresh || updated_interactions.size() > interactions.size()) {
        interactions = std::move(loadInteractions());
        updateInteractionWidgets();
        forceWidgetRefresh = false;
        startContinueButton->setEnabled(true);
      }
    }

    if (workflow->getState() != RUNNING) {
      try {
        auto shortcutsActual = processmanager_interface->retrieveShortcuts(workflow->getId());
        bool shortcutsUpdated = !std::equal(shortcuts.begin(), shortcuts.end(), shortcutsActual.begin(),
                                            shortcutsActual.end(),
                                            [](const auto &lhs, const auto &rhs) {
                                              return *lhs == *rhs;
                                            });
        if (shortcutsUpdated) {
          shortcuts = std::move(shortcutsActual);
          emit shortcutsAvailable(!shortcuts.empty());
          clearShortcutsMenu();
          generateShortcutsMenu();
        }
      } catch (const std::exception& exception) {
        statusBarInterface->showMessage("workflow interactions",
                                        tr("Unable to receive shortcuts info from process manager"));
      }
    }

    updateButtonsForCurrentState();

    if (workflow->getState() > CANCELLING) workflowRefreshTimer->stop();
  }
}

void WorkflowInteraction::updateButtonsForCurrentState() {
  if (workflow) {
    switch (workflow->getState()) {
      case READY:
        // this case applies if a workflow file has been loaded, but the workflow was not
        // started yet (and has not been given to any process_engine)
        startContinueButton->setText(tr("Start"));
        startContinueButton->setEnabled(true);
        cancel_button->setEnabled(false);
        break;
      case RUNNING:
        cancel_button->setEnabled(true);
        break;
      case NEEDS_INTERACTION:
        startContinueButton->setText(tr("Continue"));
        cancel_button->setEnabled(true);
        // not enabling the button here, to avoid that it will be spammed
        // it will be enabled as soon as new interactions are available in refreshWorkflow()
        break;
      case CANCELLED:
      case FINISHED:
      case ERROR:
        emit disableWidgets(true);
        [[fallthrough]];
      default:
        startContinueButton->setText(tr("Start"));
        startContinueButton->setEnabled(false);
        cancel_button->setEnabled(false);
    }
  } else {
    startContinueButton->setText(tr("Start"));
    startContinueButton->setEnabled(false);
    // cancel_button->setEnabled(false);
  }
}

void WorkflowInteraction::createMenu() {
  menu = new QMenu("Workflow");

  auto menu_plugin_chooser = pluginmanager->getInterface<MenuPluginChooserInterface*>("/plugins/infrastructure/menupluginchooser");
  menu_plugin_chooser->generatePluginMenu(menu, tr("&Open"), "/plugins/infrastructure/dialogs/fileopen",
                                          [this](const QString &plugin_namespace) {
    openLoadDialog(plugin_namespace);
  }, QKeySequence("Ctrl+O"), QIcon(":/studio/plugins/application/workfloweditor/icons/fa-folder-open.svg")); // TODO Icon verschieben

  recentfiles = new RecentFiles(menu, "/plugins/application/workflowinteraction");
  // recentfiles->recentfilesMenu()->setIcon(QIcon(":/studio/plugins/application/workfloweditor/icons/fa-clock-regular.svg"));
  connect(recentfiles, &RecentFiles::openRecentFile, this, qOverload<const QString&>(&WorkflowInteraction::initializeWorkflow));

  auto reloadAction = new QAction(tr("Reload"));
  reloadAction->setIcon(this->style()->standardIcon(QStyle::SP_BrowserReload));
  reloadAction->setDisabled(true);
  menu->addAction(reloadAction);
  connect(this, &WorkflowInteraction::workflowLoaded, reloadAction, [reloadAction](bool workflowLoaded) {
    reloadAction->setDisabled(!workflowLoaded); // disable menu when there is no workflow loaded
  });
  connect(reloadAction, &QAction::triggered, this, [this]() {
    initializeWorkflow(workflow->getFileName());
  });

  menu->addSeparator();

  auto historyAction = new QAction(tr("Open history"));
  historyAction->setIcon(QIcon(":/studio/plugins/application/workflowinteraction/icons/book.svg"));
  menu->addAction(historyAction);
  connect(historyAction, &QAction::triggered, this, &WorkflowInteraction::switchToExecutionTab);
}

void WorkflowInteraction::clearShortcutsMenu() {
  for (auto *action : shortcutActions) {
    shortcutsMenu->removeAction(action);
  }
}

void WorkflowInteraction::generateShortcutsMenu() {
  for (const auto &shortcut : shortcuts) {
    auto shortcutAction = new QAction(shortcut->name);
    shortcutsMenu->addAction(shortcutAction);
    shortcutActions.push_back(shortcutAction);
    auto path = shortcut->path;
    connect(shortcutAction, &QAction::triggered, this, [path] {
      QDesktopServices::openUrl(QUrl::fromLocalFile(path));
    });
  }
}

bool WorkflowInteraction::performContinueWorkflow() {
  std::vector<std::string> invalid_fields;

  if (!interactionWidget->formValid(invalid_fields)) {
    QString message = tr("Input missing! Please fill out every field to continue.") + "\n"
                    + tr("Fields with missing value: ") + "\n\n";
    for (const std::string& field_name : invalid_fields) {
      message += " - " + QString::fromStdString(field_name) + "\n";
    }
    QMessageBox::information(this, "Info", message);
  } else {
    emit disableWidgets(true);
    if (interactionWidget->sendValues()) {
      try {
        processmanager_interface->continueWorkflow(workflow->getId());
        return true;
      } catch (const std::exception& exception) {
        QMessageBox::critical(this, tr("Error"), tr("Unable to restart the workflow") + "\n" + exception.what());
      }
    }
  }
  return false;
}

bool WorkflowInteraction::performStartWorkflow() {
  // assume that the workflow fileName has been set and is not empty
  QString errortext;
  if (workflow && !workflow->getFileName().isEmpty()) {
    QString filePath = workflow->getFileName();
    FileOpenDialogInterface *matchingFileDialogInterface = FileOpenDialogInterface::getCompatibleFileOpenPlugin(pluginmanager, filePath);

    if (matchingFileDialogInterface && matchingFileDialogInterface->validateAndLoadFilePath(filePath)) {
      try {
        workflow = std::move(processmanager_interface->startWorkflow(matchingFileDialogInterface->getCachedFilePath()));

        workflowInfoWidget->setWorkflow(workflow.get());
        workflowInfoWidget->updateInfo();
        recentfiles->adjustForCurrentFile(filePath);
        return true;
      } catch (const std::exception& exception) {
        errortext = tr("The process manager failed to start the workflow!") + "\n " + exception.what();
      }
    } else {
      errortext = tr("Unable to open the selected workflow file") + "\n" + filePath;
    }
  } else {
    errortext = tr("The workflow file name was not set or is empty! Please choose a valid file via Workflow->Load.");
  }

  statusBarInterface->showMessage("workflow interactions", tr("Unable to start the workflow:") + "\n" + errortext);
  QMessageBox::critical(this, tr("Error"), tr("Unable to start the workflow:") + "\n" + errortext);
  return false;
}

bool WorkflowInteraction::performCancelWorkflow() {
  QString errortext;
  if (!workflow) {
    errortext = tr("No workflow loaded");
  } else {
    try {
      processmanager_interface->cancelWorkflow(workflow->getId());
      return true;
    } catch (const std::exception& exception) {
      errortext = tr("Failed to cancel workflow execution!") + "\n" + exception.what();
    }
  }

  QMessageBox::critical(this, tr("Error"), errortext);
  return false;
}

void WorkflowInteraction::switchToExecutionTab() {
  QString executionPluginNamespace("/plugins/application/workflowexecution");
  if (!pluginmanager->isRunning(executionPluginNamespace.toStdString())) {
    // toggle creates the plugin and switches the tab, but must not be called if the plugin is loaded already
    pluginmanager->run(executionPluginNamespace.toStdString());

  } else {
    TabInterface *tab = TabDelegate::getInstance();
    tab->setActiveTab(executionPluginNamespace);
  }
}

void WorkflowInteraction::dragEnterEvent(QDragEnterEvent* e) {
  if (e->mimeData()->hasUrls()) {
    QFileInfo fi(e->mimeData()->urls().at(0).fileName());
    if (fi.suffix() == "flow") {
      e->acceptProposedAction();
      return;
    }
  }
  e->ignore();
}

void WorkflowInteraction::dragMoveEvent(QDragMoveEvent* e) {
  if (e->mimeData()->hasUrls()) {
    QFileInfo fi(e->mimeData()->urls().at(0).fileName());
    if (fi.suffix() == "flow") {
      e->acceptProposedAction();
      return;
    }
  }
  e->ignore();
}

void WorkflowInteraction::dropEvent(QDropEvent* e) {
  if (e->mimeData()->hasUrls()) {
    QString filename = e->mimeData()->urls().at(0).fileName();
    QFileInfo fi(filename);
    if (fi.suffix() == "flow") {
      initializeWorkflow(filename);
    }
  }
}

void WorkflowInteraction::setupPrimaryButtonStyle(QPushButton* button) {
  button->setFixedSize(140, 50);
  button->setLayoutDirection(Qt::LeftToRight);
  button->setIconSize(QSize(16, 16));
  button->setStyleSheet(
    "QPushButton { text-align: left; padding-left: 20px; }"
    "QPushButton { font-size: 16px; }"
    "QPushButton:enabled { background-color: #2c3e50; color: white; }"
    "QPushButton:enabled:hover { background-color: #253544; }"
    "QPushButton:enabled:pressed { background-color: #233240; }"
  );
}

void WorkflowInteraction::setupSecondaryButtonStyle(QPushButton* button) {
  button->setFixedSize(140, 50);
  button->setLayoutDirection(Qt::LeftToRight);
  button->setIconSize(QSize(16, 16));
  button->setStyleSheet(
    "QPushButton { text-align: left; padding-left: 20px; }"
    "QPushButton { font-size: 16px; }"
    "QPushButton:enabled { color: #2c3e50; }"
    );
}

void WorkflowInteraction::setupDangerButtonStyle(QPushButton* button) {
  button->setFixedSize(140, 50);
  button->setLayoutDirection(Qt::LeftToRight);
  button->setIconSize(QSize(16, 16));
  button->setStyleSheet(
    "QPushButton { text-align: left; padding-left: 20px; }"
    "QPushButton { font-size: 16px; }"
    "QPushButton:enabled { background-color: #ec221f; color: white; }"
    "QPushButton:enabled:hover { background-color: #c00f0c; }"
    "QPushButton:enabled:pressed { background-color: #ad0d0a; }"
    );
}
