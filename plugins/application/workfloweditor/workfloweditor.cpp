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

#include <QApplication>
#include <QMenu>
#include <QVBoxLayout>
#include <QComboBox>
#include <QFileInfo>
#include <QActionGroup>

#include <framework/enhanced/recentfiles.h>

#include <QtNodes/ConnectionStyle>
#include <QtNodes/GraphicsViewStyle>
#include <QtNodes/NodeStyle>
#include <QtNodes/BasicGraphicsScene>
#include "WorkFlowGraphModel.h"

#include <framework/pluginframework/pluginchooser/pluginchooser.h>
#include <plugins/infrastructure/menupluginchooser/menupluginchooserinterface.h>
#include <plugins/infrastructure/dialogs/fileopen/fileopendialoginterface.h>
#include <plugins/infrastructure/kadiconfig/kadiconfiginterface.h>
#include <plugins/infrastructure/dialogs/registertooldialog/registertooldialoginterface.h>
#include <plugins/infrastructure/settings/settingsinterface.h>
#include <framework/tab/tabdelegate.h>
#include <framework/menu/menudelegate.h>

#include "nodes/data/workflowdatatypes.h"
#include "nodes/models/sources/booleansourcenode.h"
#include "nodes/models/sources/integersourcenode.h"
#include "nodes/models/sources/floatsourcenode.h"
#include "nodes/models/sources/stringsourcenode.h"
#include "nodes/models/misc/formatstringnode.h"
#include "nodes/models/io/fileoutputnode.h"
#include "nodes/models/user-io/userinputboolnode.h"
#include "nodes/models/user-io/userinputintegernode.h"
#include "nodes/models/user-io/userinputfloatnode.h"
#include "nodes/models/user-io/userinputtextnode.h"
#include "nodes/models/user-io/userinputfilenode.h"
#include "nodes/models/user-io/useroutputtextnode.h"
#include "nodes/models/user-io/userinputformnode.h"
#include "nodes/models/user-io/userinputcropimagesnode.h"
#include "nodes/models/user-io/userinputselectboundingboxnode.h"
#include "nodes/models/user-io/userinputchoosenode.h"
#include "nodes/models/user-io/userinputselectnode.h"
#include "nodes/models/user-io/userinputperiodictablenode.h"
#include "nodes/models/user-io/useroutputwebviewnode.h"
#include "nodes/models/control/variablenode.h"
#include "nodes/models/control/variablejsonnode.h"
#include "nodes/models/control/variablelistnode.h"
#include "nodes/models/control/ifnode.h"
#include "nodes/models/control/loopnode.h"
#include "nodes/models/control/branchselectnode.h"
#include "nodes/models/toolnode.h"
#include "nodes/models/envnode.h"
#include "nodes/models/notenode.h"
#include "nodes/models/io/fileinputnode.h"

#ifdef WEBVIEW_SUPPORT_ENABLED
#include "nodes/models/user-io/useroutputwebviewnode.h"
#endif

#include "workfloweditor.h"
#include "workflowview.h"
#include "workflowscene.h"
#include "config.h"

using QtNodes::ConnectionStyle;
using QtNodes::GraphicsViewStyle;
static void registerDataModels(NodeDelegateModelRegistry* ret) {
  QString source = "Source",
          control = "Control",
          file_io = "File IO",
          user_input = "User Input",
          user_output = "User Output",
          annotation = "Annotation",
          misc = "Misc";
  ret->registerModel<StringSourceNode>(source);
  ret->registerModel<FloatSourceNode>(source);
  ret->registerModel<IntegerSourceNode>(source);
  ret->registerModel<BooleanSourceNode>(source);

  ret->registerModel<VariableNode>(control);
  ret->registerModel<VariableJsonNode>(control);
  ret->registerModel<VariableListNode>(control);
  ret->registerModel<IfNode>(control);
  ret->registerModel<LoopNode>(control);
  ret->registerModel<BranchSelectNode>(control);

  ret->registerModel<FileOutputNode>(file_io);
  ret->registerModel<FileInputNode>(file_io);

  ret->registerModel<UserInputTextNode>(user_input);
  ret->registerModel<UserInputFileNode>(user_input);
  ret->registerModel<UserInputFormNode>(user_input);
  ret->registerModel<UserInputCropImagesNode>(user_input);
  ret->registerModel<UserInputSelectBoundingBoxNode>(user_input);
  ret->registerModel<UserInputIntegerNode>(user_input);
  ret->registerModel<UserInputFloatNode>(user_input);
  ret->registerModel<UserInputBoolNode>(user_input);
  ret->registerModel<UserInputChooseNode>(user_input);
  ret->registerModel<UserInputSelectNode>(user_input);
  ret->registerModel<UserInputPeriodicTableNode>(user_input);

  ret->registerModel<UserOutputTextNode>(user_output);
#ifdef WEBVIEW_SUPPORT_ENABLED
  ret->registerModel<UserOutputWebViewNode>(user_output);
#endif

  ret->registerModel<NoteNode>(annotation);

  ret->registerModel<FormatStringNode>(misc);

  // the following entries will not be listed in the context menu, but must be registered to be available
  // for constructing nodes programmatically:
  ret->registerModel<ToolNode>(WorkflowView::EXCLUDED_CATEGORY);
  ret->registerModel<EnvNode>(WorkflowView::EXCLUDED_CATEGORY);
}

static void registerTypeConverters(WorkFlowGraphModel* ret) {
  auto emptyConverter = [](const std::shared_ptr<NodeData>& p) -> std::shared_ptr<NodeData> {
    // Nothing to do here, the conversation of data takes place in the process engine!
    return p;
  };

  // allow common data conversions
  ret->registerTypeConverter(std::make_pair(DataTypes::STRING, DataTypes::INTEGER), emptyConverter);
  ret->registerTypeConverter(std::make_pair(DataTypes::STRING, DataTypes::FLOAT), emptyConverter);
  ret->registerTypeConverter(std::make_pair(DataTypes::STRING, DataTypes::BOOLEAN), emptyConverter);

  ret->registerTypeConverter(std::make_pair(DataTypes::FLOAT, DataTypes::STRING), emptyConverter);
  ret->registerTypeConverter(std::make_pair(DataTypes::INTEGER, DataTypes::STRING), emptyConverter);
  ret->registerTypeConverter(std::make_pair(DataTypes::BOOLEAN, DataTypes::STRING), emptyConverter);

  ret->registerTypeConverter(std::make_pair(DataTypes::INTEGER, DataTypes::FLOAT), emptyConverter);
  ret->registerTypeConverter(std::make_pair(DataTypes::FLOAT, DataTypes::INTEGER), emptyConverter);

  // allow special conversions with pipes, the process engine supports only this direction (pipe -> *) by now
  ret->registerTypeConverter(std::make_pair(DataTypes::PIPE, DataTypes::STRING), emptyConverter);
  ret->registerTypeConverter(std::make_pair(DataTypes::PIPE, DataTypes::INTEGER), emptyConverter);
  ret->registerTypeConverter(std::make_pair(DataTypes::PIPE, DataTypes::FLOAT), emptyConverter);
  ret->registerTypeConverter(std::make_pair(DataTypes::PIPE, DataTypes::BOOLEAN), emptyConverter);
}

static void setStyle() {
  GraphicsViewStyle::setStyle(EditorConfig::GRAPHICS_VIEW_STYLE);
  NodeStyle::setNodeStyle(EditorConfig::NODE_STYLE);
  ConnectionStyle::setConnectionStyle(EditorConfig::CONNECTION_STYLE);
}

Workfloweditor::Workfloweditor(LibFramework::PluginManagerInterface* pluginmanager, QObject* parent)
    : QObject(parent) {
  this->pluginmanager = pluginmanager;

  setStyle();
  auto registry = std::make_shared<NodeDelegateModelRegistry>();
  registerDataModels(registry.get());
  WorkFlowGraphModel *workflowgraphmodel = new WorkFlowGraphModel(registry);
  registerTypeConverters(workflowgraphmodel);
  scene = new WorkflowScene(*workflowgraphmodel, parent);
  connect(scene, &BasicGraphicsScene::nodeContextMenu, this, [&](NodeId const nodeId, QPointF const pos) {
    WorkflowNode *workflowNode = scene->getWorkFlowGraphModel().delegateModel<WorkflowNode>(nodeId);
    if (workflowNode) {
      QMenu nodeContextMenu;
      std::vector<NodeId> selectedNodes = scene->selectedNodes();
      if (std::find(selectedNodes.begin(), selectedNodes.end(), nodeId) == selectedNodes.end()) {
        // node is not in the selected list
        scene->clearSelection();
        selectedNodes = scene->selectedNodes();
      }
      QString label = (selectedNodes.size() > 1) ? "%1 selected" : "%1";

      QAction *copyAction = nodeContextMenu.addAction(label.arg("Copy"));
      copyAction->setShortcut(QKeySequence::Copy);
      copyAction->setShortcutVisibleInContextMenu(true);
      connect(copyAction, &QAction::triggered, this, [&]() {
        // make sure that the trigged element is selected
        scene->nodeGraphicsObject(nodeId)->setSelected(true);
        view->onCopySelectedObjects();
      });
      nodeContextMenu.addAction(copyAction);

      QAction *deleteAction = nodeContextMenu.addAction(label.arg("Remove"));
      deleteAction->setShortcut(QKeySequence::Delete);
      deleteAction->setShortcutVisibleInContextMenu(true);
      connect(deleteAction, &QAction::triggered, this, [&]() {
        // make sure that the trigged element is selected
        scene->nodeGraphicsObject(nodeId)->setSelected(true);
        view->onDeleteSelectedObjects();
      });
      nodeContextMenu.addAction(deleteAction);

      nodeContextMenu.exec(view->mapToGlobal(view->mapFromScene(pos.toPoint())));
    }
  });
  view = new WorkflowView(scene, dynamic_cast<QWidget*>(parent), pluginmanager);

  workflowMenu = createWorkflowMenu();
  viewMenu = createViewMenu();
  toolsMenu = createToolsMenu();
  toolbar = createToolBar(view);
}


QMenu* Workfloweditor::createWorkflowMenu() {
  auto workflowMenu = new QMenu(tr("File"));
  workflowMenu->setToolTipsVisible(true);

  auto new_action = new QAction(tr("New"));
  new_action->setShortcut(QKeySequence::New);
  new_action->setIcon(QIcon(":/studio/plugins/application/workfloweditor/icons/fa-file.svg"));
  connect(new_action, &QAction::triggered, view, &WorkflowView::clearWorkflow);

  auto save_action = new QAction(tr("Save"));
  save_action->setShortcut(QKeySequence::Save);
  save_action->setEnabled(false);
  save_action->setIcon(QIcon(":/studio/plugins/application/workfloweditor/icons/content-save-outline.svg"));
  connect(save_action, &QAction::triggered, view, &WorkflowView::saveWorkflow);
  connect(scene, &WorkflowScene::saveFileSet, save_action, &QAction::setEnabled);

  auto save_as_action = new QAction(tr("Save as"));
  save_as_action->setShortcut(QKeySequence::SaveAs);
  save_as_action->setIcon(QIcon(":/studio/plugins/application/workfloweditor/icons/content-save-edit-outline.svg"));
  connect(save_as_action, &QAction::triggered, view, &WorkflowView::saveWorkflowAs);

  auto upload_action = new QAction(tr("Upload to Kadi"));
  upload_action->setShortcut(QKeySequence("Shift+Ctrl+K"));
  upload_action->setIcon(QIcon(":/studio/plugins/application/workfloweditor/icons/fa-cloud-arrow-up.svg"));
  connect(upload_action, &QAction::triggered, view, &WorkflowView::upload);

  auto exportmenu = new QMenu(tr("Export as"));
  exportmenu->setIcon(QIcon(":/studio/plugins/application/workfloweditor/icons/camera-solid.svg"));
  auto screenshot_image_action = new QAction("image");
  // screenshot_image_action->setShortcut(QKeySequence::("Shift+Ctrl+K"));
  connect(screenshot_image_action, &QAction::triggered, scene, &WorkflowScene::exportAsPng);
  auto screenshot_svg_action = new QAction("vector (SVG/PDF)");
  // screenshot_svg_action->setShortcut(QKeySequence("Shift+Ctrl+K"));
  connect(screenshot_svg_action, &QAction::triggered, scene, &WorkflowScene::exportAsSvg);

  auto workflow_variables_action = new QAction("Edit Variables");
  workflow_variables_action->setShortcut(QKeySequence("Shift+V"));
  workflow_variables_action->setIcon(QIcon(":/studio/plugins/application/workfloweditor/icons/fa-rectangle-list.svg"));
  connect(workflow_variables_action, &QAction::triggered, view, &WorkflowView::openVariablesDialog);

  auto execute_action = new QAction("Execute");
  execute_action->setShortcut(QKeySequence("Ctrl+E"));
  execute_action->setIcon(QIcon(":/studio/plugins/application/workfloweditor/icons/fa-play.svg"));
  connect(execute_action, &QAction::triggered, view, &WorkflowView::startExecution);

  workflowMenu->addAction(new_action);

  auto menu_plugin_chooser = pluginmanager->getInterface<MenuPluginChooserInterface*>("/plugins/infrastructure/menupluginchooser");
  menu_plugin_chooser->generatePluginMenu(workflowMenu, "&Open", "/plugins/infrastructure/dialogs/fileopen",
                                          [this](const QString &plugin_namespace) {
    view->openLoadDialog(plugin_namespace);
  }, QKeySequence::Open, QIcon(":/studio/plugins/application/workfloweditor/icons/fa-folder-open.svg"));

  RecentFiles *recentfiles = new RecentFiles(workflowMenu, "/plugins/application/workfloweditor");
  recentfiles->recentfilesMenu()->setIcon(QIcon(":/studio/plugins/application/workfloweditor/icons/fa-clock-regular.svg"));
  connect(scene, &WorkflowScene::filepathChanged, this, [this, recentfiles](QString filepath) {
    if (filepath.isEmpty()) {
      setTabName("Unnamed workflow");
    } else {
      QFileInfo workflowSaveFile(filepath);
      setTabName(workflowSaveFile.fileName());
      recentfiles->adjustForCurrentFile(filepath);
    }
  });
  connect(recentfiles, &RecentFiles::openRecentFile, this, &Workfloweditor::openWorkflow);

  menu_plugin_chooser->generatePluginMenu(workflowMenu, "&Merge", "/plugins/infrastructure/dialogs/fileopen",
                                          [this](const QString &plugin_namespace) {
    PluginChooser pluginchooser(view);
    auto fileDialog = pluginchooser.chooseFrom<FileOpenDialogInterface *>(pluginmanager, plugin_namespace);
    if (!fileDialog) {
      return;
    }
    fileDialog->setFileMode(FileOpenDialogInterface::ExistingFile);
    fileDialog->applyFilter(tr("Workflow Files (*.flow)"));
    if (fileDialog->showFileOpenDialog()) {
      QString filepath = fileDialog->getFilePath();

      auto bb = scene->itemsBoundingRect();
      QPointF offset = {bb.left()+40.0, bb.bottom()};

      view->merge(filepath, offset);
    }

  }, QKeySequence("Ctrl+M"), QIcon(":/studio/plugins/application/workfloweditor/icons/fa-folder-open.svg"));

  workflowMenu->addAction(save_action);
  workflowMenu->addAction(save_as_action);
  workflowMenu->addAction(upload_action);
  workflowMenu->addMenu(exportmenu);
  exportmenu->addAction(screenshot_image_action);
  exportmenu->addAction(screenshot_svg_action);

  workflowMenu->addSeparator();
  workflowMenu->addAction(workflow_variables_action);
  workflowMenu->addAction(execute_action);
  return workflowMenu;
}

QMenu* Workfloweditor::createViewMenu() {
  auto viewMenu = new QMenu("View");

  auto view_zoom_in_action = new QAction("Zoom In");
  view_zoom_in_action->setShortcut(QKeySequence::ZoomIn);
  view_zoom_in_action->setIcon(QIcon(":/studio/plugins/application/workfloweditor/icons/magnifying-glass-plus-solid.svg"));
  connect(view_zoom_in_action, &QAction::triggered, view, &WorkflowView::scaleUp);

  auto view_zoom_out_action = new QAction("Zoom Out");
  view_zoom_out_action->setShortcut(QKeySequence::ZoomOut);
  view_zoom_out_action->setIcon(QIcon(":/studio/plugins/application/workfloweditor/icons/magnifying-glass-minus-solid.svg"));
  connect(view_zoom_out_action, &QAction::triggered, view, &WorkflowView::scaleDown);

  auto view_reset_action = new QAction("Reset");
  view_reset_action->setShortcut(QKeySequence("Ctrl+R"));
  view_reset_action->setIcon(QIcon(":/studio/plugins/application/workfloweditor/icons/fa-eye.svg"));
  connect(view_reset_action, &QAction::triggered, view, &WorkflowView::resetView);

  auto show_grid_action = new QAction("Show grid");
  show_grid_action->setShortcut(QKeySequence("Ctrl+G"));
  show_grid_action->setIcon(QIcon(":/studio/plugins/application/workfloweditor/icons/fa-border-all.svg"));
  show_grid_action->setCheckable(true);
  // show_grid_action->setChecked(grid_visible);
  connect(show_grid_action, &QAction::triggered, view, &WorkflowView::showGrid);
  connect(view, &WorkflowView::gridVisibilityChanged, show_grid_action, &QAction::setChecked);

  auto auto_layout_action = new QAction("Auto layout");
  auto_layout_action->setShortcut(QKeySequence("Ctrl+L"));
  auto_layout_action->setIcon(QIcon(":/studio/plugins/application/workfloweditor/icons/fa-wand-sparkles.svg"));
  connect(auto_layout_action, &QAction::triggered, view, &WorkflowView::autoLayout);

  auto viewModeGroup = new QActionGroup(viewMenu);
  auto viewAllAction = new QAction("View all nodes");
  viewAllAction->setCheckable(true);
  viewAllAction->setShortcut(QKeySequence("Ctrl+1"));
  connect(viewAllAction, &QAction::triggered, view, [this] (bool checked) {
    if (checked) Q_EMIT view->viewModeChanged(SHOW_ALL);
  });
  connect(view, &WorkflowView::viewModeChanged, view, [viewAllAction] (ViewMode mode) {
    if (mode == SHOW_ALL) viewAllAction->setChecked(true);
  });
  auto hideInputsAction = new QAction("Hide Inputs");
  hideInputsAction->setCheckable(true);
  hideInputsAction->setShortcut(QKeySequence("Ctrl+2"));
  connect(hideInputsAction, &QAction::triggered, view, [this] (bool checked) {
    if (checked) Q_EMIT view->viewModeChanged(ViewMode::HIDE_INPUTS);
  });
  connect(view, &WorkflowView::viewModeChanged, view, [hideInputsAction] (ViewMode mode) {
    if (mode == HIDE_INPUTS) hideInputsAction->setChecked(true);
  });
  auto hideUserInputsAction = new QAction("Hide User-Inputs");
  hideUserInputsAction->setCheckable(true);
  hideUserInputsAction->setShortcut(QKeySequence("Ctrl+3"));
  connect(hideUserInputsAction, &QAction::triggered, view, [this] (bool checked) {
    if (checked) Q_EMIT view->viewModeChanged(ViewMode::HIDE_USER_INPUTS);
  });
  connect(view, &WorkflowView::viewModeChanged, view, [hideUserInputsAction] (ViewMode mode) {
    if (mode == HIDE_USER_INPUTS) hideUserInputsAction->setChecked(true);
  });
  auto hideInputsAndUserInputsAction = new QAction("Hide Inputs && User-Inputs");
  hideInputsAndUserInputsAction->setCheckable(true);
  hideInputsAndUserInputsAction->setShortcut(QKeySequence("Ctrl+4"));
  connect(hideInputsAndUserInputsAction, &QAction::triggered, view, [this] (bool checked) {
    if (checked) Q_EMIT view->viewModeChanged(ViewMode::HIDE_INPUTS_AND_USER_INPUTS);
  });
  connect(view, &WorkflowView::viewModeChanged, view, [hideInputsAndUserInputsAction] (ViewMode mode) {
    if (mode == HIDE_INPUTS_AND_USER_INPUTS) hideInputsAndUserInputsAction->setChecked(true);
  });
  viewModeGroup->addAction(viewAllAction);
  viewModeGroup->addAction(hideInputsAction);
  viewModeGroup->addAction(hideUserInputsAction);
  viewModeGroup->addAction(hideInputsAndUserInputsAction);

  viewMenu->addAction(view_zoom_in_action);
  viewMenu->addAction(view_zoom_out_action);
  viewMenu->addAction(view_reset_action);
  viewMenu->addAction(show_grid_action);
  viewMenu->addAction(auto_layout_action);
  viewMenu->addSeparator()->setText("View Mode");
  viewMenu->addAction(viewAllAction);
  viewMenu->addAction(hideInputsAction);
  viewMenu->addAction(hideUserInputsAction);
  viewMenu->addAction(hideInputsAndUserInputsAction);

  viewAllAction->setChecked(true);

  return viewMenu;
}

QMenu* Workfloweditor::createToolsMenu() const {
  auto toolsMenu = new QMenu("Tools and Settings");

  auto kadi_config_action = new QAction("Edit Kadi Config");
  kadi_config_action->setShortcut(QKeySequence("Alt+Ctrl+K"));
  kadi_config_action->setIcon(QIcon(":/studio/framework/pixmaps/kadi_logo.svg"));
  connect(kadi_config_action, &QAction::triggered, this, &Workfloweditor::showKadiConfig);

  toolsMenu->addAction(kadi_config_action);

  auto register_tool_action = new QAction("Register External Tools");
  register_tool_action->setShortcut(QKeySequence("Ctrl+T"));
  register_tool_action->setIcon(QIcon(":/studio/plugins/application/workfloweditor/icons/fa-toolbox.svg"));
  connect(register_tool_action, &QAction::triggered, this, &Workfloweditor::registerTool);

  toolsMenu->addAction(register_tool_action);

  toolsMenu->addSeparator();

  // view->getSettings()->addConfigMenu("/plugins/application/workfloweditor", toolsMenu);

  return toolsMenu;
}

QToolBar* Workfloweditor::createToolBar(QWidget* parent) {
  QToolBar *toolbar = new QToolBar(tr("Toolbar"), parent);
  toolbar->setIconSize(QSize(20, 20));
  toolbar->setStyleSheet("QToolBar{spacing:10px;}");
  toolbar->setFixedHeight(30);
  toolbar->setMovable(false);

  auto new_action = new QAction("New");
  new_action->setIcon(QIcon(":/studio/plugins/application/workfloweditor/icons/fa-file.svg"));
  connect(new_action, &QAction::triggered, view, &WorkflowView::clearWorkflow);
  toolbar->addAction(new_action);

  auto menu_plugin_chooser = pluginmanager->getInterface<MenuPluginChooserInterface*>("/plugins/infrastructure/menupluginchooser");
  menu_plugin_chooser->generatePluginToolbarItem(toolbar, "Open Workflow",
                                                 QIcon(":/studio/plugins/application/workfloweditor/icons/fa-folder-open.svg"),
                                                 "/plugins/infrastructure/dialogs/fileopen",
                                                 [this](const QString& plugin_namespace) {
                                                   view->openLoadDialog(plugin_namespace);
                                                 });

  auto save_action = new QAction("Save");
  save_action->setIcon(QIcon(":/studio/plugins/application/workfloweditor/icons/content-save-outline.svg"));
  save_action->setEnabled(false);
  connect(save_action, &QAction::triggered, view, &WorkflowView::saveWorkflow);
  connect(scene, &WorkflowScene::saveFileSet, save_action, &QAction::setEnabled);
  toolbar->addAction(save_action);

  auto save_as_action = new QAction("Save as");
  save_as_action->setIcon(QIcon(":/studio/plugins/application/workfloweditor/icons/content-save-edit-outline.svg"));
  connect(save_as_action, &QAction::triggered, view, &WorkflowView::saveWorkflowAs);
  toolbar->addAction(save_as_action);

  auto upload_action = new QAction("Upload to Kadi");
  upload_action->setIcon(QIcon(":/studio/plugins/application/workfloweditor/icons/fa-cloud-arrow-up.svg"));
  connect(upload_action, &QAction::triggered, view, &WorkflowView::upload);
  toolbar->addAction(upload_action);

  toolbar->addSeparator();

  auto kadi_config_action = new QAction("Edit Kadi Config");
  kadi_config_action->setIcon(QIcon(":/studio/framework/pixmaps/kadi_logo.svg"));
  connect(kadi_config_action, &QAction::triggered, this, &Workfloweditor::showKadiConfig);
  toolbar->addAction(kadi_config_action);

  auto register_tool_action = new QAction("Register External Tools");
  register_tool_action->setIcon(QIcon(":/studio/plugins/application/workfloweditor/icons/fa-toolbox.svg"));
  connect(register_tool_action, &QAction::triggered, this, &Workfloweditor::registerTool);
  toolbar->addAction(register_tool_action);

  auto workflow_variables_action = new QAction("Edit Variables");
  workflow_variables_action->setIcon(QIcon(":/studio/plugins/application/workfloweditor/icons/fa-rectangle-list.svg"));
  connect(workflow_variables_action, &QAction::triggered, view, &WorkflowView::openVariablesDialog);
  toolbar->addAction(workflow_variables_action);

  toolbar->addSeparator();

  auto execute_action = new QAction("Execute this workflow");
  execute_action->setIcon(QIcon(":/studio/plugins/application/workfloweditor/icons/fa-play.svg"));
  connect(execute_action, &QAction::triggered, view, &WorkflowView::startExecution);
  toolbar->addAction(execute_action);

  auto spacer = new QWidget();
  spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
  toolbar->addWidget(spacer);

  auto undo_action = scene->undoStack().createUndoAction(this);
  undo_action->setIcon(QIcon(":/studio/plugins/application/workfloweditor/icons/arrow-rotate-left-solid.svg"));
  toolbar->addAction(undo_action);

  auto redo_action = scene->undoStack().createRedoAction(this);
  redo_action->setIcon(QIcon(":/studio/plugins/application/workfloweditor/icons/arrow-rotate-right-solid.svg"));
  toolbar->addAction(redo_action);

  toolbar->addSeparator();

  auto auto_layout_action = new QAction("Auto layout");
  auto_layout_action->setIcon(QIcon(":/studio/plugins/application/workfloweditor/icons/fa-wand-sparkles.svg"));
  connect(auto_layout_action, &QAction::triggered, view, &WorkflowView::autoLayout);
  toolbar->addAction(auto_layout_action);

  auto view_zoom_in_action = new QAction("Zoom In");
  view_zoom_in_action->setShortcut(QKeySequence::ZoomIn);
  view_zoom_in_action->setIcon(QIcon(":/studio/plugins/application/workfloweditor/icons/magnifying-glass-plus-solid.svg"));
  connect(view_zoom_in_action, &QAction::triggered, view, &WorkflowView::scaleUp);
  toolbar->addAction(view_zoom_in_action);

  auto view_zoom_out_action = new QAction("Zoom Out");
  view_zoom_out_action->setShortcut(QKeySequence::ZoomOut);
  view_zoom_out_action->setIcon(QIcon(":/studio/plugins/application/workfloweditor/icons/magnifying-glass-minus-solid.svg"));
  connect(view_zoom_out_action, &QAction::triggered, view, &WorkflowView::scaleDown);
  toolbar->addAction(view_zoom_out_action);

  auto reset_view_action = new QAction("Reset view");
  reset_view_action->setIcon(QIcon(":/studio/plugins/application/workfloweditor/icons/fa-eye.svg"));
  connect(reset_view_action, &QAction::triggered, view, &WorkflowView::resetView);
  toolbar->addAction(reset_view_action);

  auto show_grid_action = new QAction("Show grid");
  show_grid_action->setIcon(QIcon(":/studio/plugins/application/workfloweditor/icons/fa-border-all.svg"));
  show_grid_action->setCheckable(true);
  connect(show_grid_action, &QAction::triggered, view, &WorkflowView::showGrid);
  connect(view, &WorkflowView::gridVisibilityChanged, show_grid_action, &QAction::setChecked);
  toolbar->addAction(show_grid_action);

  toolbar->addSeparator();

  auto view_mode_combobox = new QComboBox();
  view_mode_combobox->addItems({tr("View all nodes"), tr("Hide Inputs"), tr("Hide User-Inputs"), tr("Hide Inputs & User-Inputs")});
  toolbar->addWidget(view_mode_combobox);

  QSlider *opacity_slider = new QSlider(Qt::Orientation::Horizontal);
  opacity_slider->setMaximum(100);
  opacity_slider->setMaximumWidth(150);
  opacity_slider->setToolTip(tr("Set opacity for hidden nodes and their connections"));
  connect(opacity_slider, &QSlider::valueChanged, view, &WorkflowView::setViewModeOpacity);
  connect(view, &WorkflowView::viewModeOpacityChanged, opacity_slider, &QSlider::setValue);
  connect(view_mode_combobox, qOverload<int>(&QComboBox::currentIndexChanged), this, [this] (int index) {
    view->setViewMode((ViewMode) index);
  });
  connect(view, &WorkflowView::viewModeChanged, this, [view_mode_combobox, opacity_slider] (ViewMode mode) {
    view_mode_combobox->setCurrentIndex((int) mode);
    opacity_slider->setDisabled(mode == SHOW_ALL);
  });

  toolbar->addWidget(opacity_slider);

  return toolbar;
}

void Workfloweditor::openWorkflow(const QString& filepath) {
  if (view->checkUnsavedValues()) return;
  view->load(filepath);
}

void Workfloweditor::setTabName(const QString& name) {
  TabInterface *tab = TabDelegate::getInstance();
  tab->setTabName("/plugins/application/workfloweditor", tr("Workfloweditor")  + " - " + name);
}

void Workfloweditor::addTab() {
  TabInterface *tab = TabDelegate::getInstance();
  tab->addTab("/plugins/application/workfloweditor", view, tr("Unnamed workflow"));
  tab->addToolBar("/plugins/application/workfloweditor", toolbar);
}

void Workfloweditor::addMenus() {
  auto *menuinterface = MenuDelegate::getInstance();
  menuinterface->addMenu("/plugins/application/workfloweditor", workflowMenu);
  menuinterface->addMenu("/plugins/application/workfloweditor", viewMenu);
  menuinterface->addMenu("/plugins/application/workfloweditor", toolsMenu);
}

void Workfloweditor::showKadiConfig() {
  auto kadiConfigInterface = pluginmanager->getInterface<KadiConfigInterface *>("/plugins/infrastructure/kadiconfig");
  kadiConfigInterface->showDialog();
}

void Workfloweditor::registerTool() {
  auto registerToolDialogInterface = pluginmanager->getInterface<RegisterToolDialogInterface *>("/plugins/infrastructure/dialogs/registertooldialog");
  registerToolDialogInterface->showDialog();
}
