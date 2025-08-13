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

#include <unordered_set>

#include <QDebug>
#include <QApplication>
#include <QVBoxLayout>
#include <QContextMenuEvent>
#include <QMessageBox>
#include <QFileDialog>
#include <QFileInfo>
#include <QJsonDocument>
#include <QClipboard>
#include <QMimeData>
#include <QGraphicsOpacityEffect>
#include <QPropertyAnimation>

#include <QtNodes/NodeDelegateModel>
#include <QtNodes/AbstractGraphModel>
#include "WorkFlowGraphModel.h"
#include <framework/tab/tabdelegate.h>
#include <framework/pluginframework/pluginmanager.h>
#include <framework/pluginframework/pluginchooser/pluginchooser.h>
#include <plugins/application/workflowinteraction/WorkflowInteractionInterface.h>
#include <plugins/infrastructure/kadiintegration/kadiintegrationinterface.h>
#include <plugins/infrastructure/kadiintegration/uploadtokadidialoginterface.h>
#include <plugins/infrastructure/dialogs/fileopen/fileopendialoginterface.h>
#include <plugins/infrastructure/dialogs/editvariablesdialog/editvariablesdialoginterface.h>
#include <plugins/infrastructure/settings/settingsinterface.h>

#include "nodes/models/sources/sourcenode.h"
#include "nodes/models/user-io/userinteractionnode.h"

#include "autolayout.h"
#include "workflowscene.h"
#include "workflowview.h"

using QtNodes::NodeId;
using QtNodes::NodeGraphicsObject;
using QtNodes::NodeDelegateModel;

const QString WorkflowView::EXCLUDED_CATEGORY = "[Excluded from context menu]";

WorkflowView::WorkflowView(BasicGraphicsScene *scene, QWidget *parent, LibFramework::PluginManagerInterface *pluginmanager)
    : GraphicsView(scene, parent), pluginmanager(pluginmanager) {

  workflow_scene = (WorkflowScene*) this->scene();

  setFrameShape(QFrame::NoFrame);

  setLayout(new QVBoxLayout());

  settingsinterface = pluginmanager->getInterface<SettingsInterface*>("/plugins/infrastructure/settingsplugin");

  selectallAction = new QAction("Select All", this);
  selectallAction->setShortcut(QKeySequence::SelectAll);
  selectallAction->setShortcutVisibleInContextMenu(true);
  selectallAction->setShortcutContext(Qt::ShortcutContext::WidgetShortcut);
  addAction(selectallAction);
  connect(selectallAction, &QAction::triggered, workflow_scene, &WorkflowScene::selectAllNodes);

  connect(&scene->graphModel(), &QtNodes::AbstractGraphModel::connectionCreated, this, [this] (ConnectionId const connectionId) {
    applyViewModeOpacityForConnection(connectionId);
  });

  setAcceptDrops(true);

  loadSettings();
}

void WorkflowView::loadSettings() {
  showGrid(settingsinterface->getPluginSetting("/plugins/application/workfloweditor", "is_checked", true).toBool());
  setViewMode((ViewMode) settingsinterface->getPluginSetting("/plugins/application/workfloweditor", "view_mode", SHOW_ALL).toInt());
  setViewModeOpacity(settingsinterface->getPluginSetting("/plugins/application/workfloweditor", "view_mode_opacity", 60).toInt());
}

void WorkflowView::resizeEvent(QResizeEvent*) {
  if (grid_visible) {
    resetCachedContent();
  }
}

void WorkflowView::contextMenuEvent(QContextMenuEvent* event) {

  if (itemAt(event->pos())) {
    GraphicsView::contextMenuEvent(event);
  } else if (pluginmanager != nullptr) {

    QMenu modelMenu;

    QAction *addNodeAction = modelMenu.addAction(tr("Add tool"));
    connect(addNodeAction, &QAction::triggered, workflow_scene, [&]() {
      try {
        auto nodeId = workflow_scene->getWorkFlowGraphModel().addNode("ToolNode");
        if (nodeId != QtNodes::InvalidNodeId) {
          QPoint pos = event->pos();
          QPointF posView = this->mapToScene(pos);
          workflow_scene->undoStack().push(new NewNodeCommand(workflow_scene, nodeId, posView));

          applyViewModeOpacityForNode(nodeId);
        }
      } catch(const std::exception& e) {
        errorDialog(tr("Could not insert node"), tr("Error: Unsupported tool: \n%1").arg(e.what()));
      }
    });
    modelMenu.addSeparator();

    QHash<QString, QMenu*> menuMap;

    // create a submenu for each node model category
    // the registry returns an unordered map, so create the submenu whenever a new category
    // name comes up (and store them in a hashmap)
    for (auto const& assoc : workflow_scene->getWorkFlowGraphModel().dataModelRegistry()->registeredModelsCategoryAssociation()) {
      QString modelName = assoc.first;
      QString categoryName = assoc.second;

      if (categoryName == EXCLUDED_CATEGORY) {
        continue; // exclude this model from the context menu
      }

      if (!menuMap.contains((categoryName))) {
        menuMap[categoryName] = modelMenu.addMenu(tr("Add ") + categoryName);
      }
      menuMap[categoryName]->addAction(modelName);
    }

    connect(&modelMenu, &QMenu::triggered, [&](QAction* action) {
      QString modelName = action->text();
      auto nodeId = workflow_scene->getWorkFlowGraphModel().addNode(modelName);

      if (nodeId != QtNodes::InvalidNodeId) {
        QPoint pos = event->pos();
        QPointF posView = this->mapToScene(pos);
        workflow_scene->undoStack().push(new NewNodeCommand(workflow_scene, nodeId, posView));

        applyViewModeOpacityForNode(nodeId);
      }
      modelMenu.close();
    });

    modelMenu.addSeparator();
    // auto *selectAllNodesMenuAction = new QAction("Select &All");
    // selectAllNodesMenuAction->setShortcut(QKeySequence::SelectAll);
    // selectAllNodesMenuAction->setShortcutVisibleInContextMenu(true);
    // connect(selectAllNodesMenuAction, &QAction::triggered, workflow_scene, &WorkflowScene::selectAllNodes);
    modelMenu.addAction(selectallAction);

    auto *copyMenuAction = new QAction("&Copy");
    copyMenuAction->setDisabled(scene()->selectedItems().empty());
    copyMenuAction->setShortcut(QKeySequence::Copy);
    copyMenuAction->setShortcutVisibleInContextMenu(true);
    connect(copyMenuAction, &QAction::triggered, this, &WorkflowView::onCopySelectedObjects);
    modelMenu.addAction(copyMenuAction);

    auto *pasteMenuAction = new QAction("Paste");
    QClipboard const *clipboard = QApplication::clipboard();
    pasteMenuAction->setEnabled(clipboard->mimeData()->hasFormat("application/qt-nodes-graph"));
    pasteMenuAction->setShortcut(QKeySequence::Paste);
    pasteMenuAction->setShortcutVisibleInContextMenu(true);
    connect(pasteMenuAction, &QAction::triggered, this, &WorkflowView::onPasteObjects);
    modelMenu.addAction(pasteMenuAction);

    auto *removeAction = new QAction("Remove");
    removeAction->setDisabled(scene()->selectedItems().empty());
    removeAction->setShortcut(Qt::Key_Delete);
    removeAction->setShortcutVisibleInContextMenu(true);
    connect(removeAction, &QAction::triggered, this, &WorkflowView::onDeleteSelectedObjects);
    modelMenu.addAction(removeAction);

    modelMenu.exec(event->globalPos());
  }
}

void WorkflowView::drawBackground(QPainter* painter, const QRectF& r) {
  if (grid_visible) {
    GraphicsView::drawBackground(painter, r);
  } else {
    QGraphicsView::drawBackground(painter, r);
  }
}

void WorkflowView::resetView() {
  scene()->setSceneRect(QRect());
  setSceneRect(QRect());

  QRectF targetRect = scene()->itemsBoundingRect();

  // make sure the view is not zoomed in too much
  if (targetRect.width() < RESET_VIEW_MIN_WIDTH) {
    int width = targetRect.width();
    targetRect.setWidth(RESET_VIEW_MIN_WIDTH);
    targetRect.setLeft(targetRect.x()+(RESET_VIEW_MIN_WIDTH-width)/2);
  }
  if (targetRect.height() < RESET_VIEW_MIN_HEIGHT) {
    int height = targetRect.height();
    targetRect.setHeight(RESET_VIEW_MIN_HEIGHT);
    targetRect.setTop(targetRect.y()+(RESET_VIEW_MIN_HEIGHT-height)/2);
  }
  fitInView(targetRect, Qt::KeepAspectRatio);

  QPointF center = targetRect.center();
  centerOn(center.x(), center.y());
}

void WorkflowView::showGrid(bool isChecked) {
  settingsinterface->setPluginSetting("/plugins/application/workfloweditor", "is_checked", isChecked);
  grid_visible = isChecked;
  Q_EMIT gridVisibilityChanged(grid_visible);
  resetCachedContent();
}

void WorkflowView::switchToPlugin(const QString& pluginname) { // TODO in pluginmanager/framework
  if (!pluginmanager->isRunning(pluginname.toStdString())) {
    pluginmanager->run(pluginname.toStdString());
  } else {
    TabInterface *tab = TabDelegate::getInstance();
    tab->setActiveTab(pluginname);
  }
}

void WorkflowView::startExecution() {
  QString interactionPluginNamespace("/plugins/application/workflowinteraction");
  QString errormsg{};

  auto *workflowInteraction =
    pluginmanager->getInterface<WorkflowInteractionInterface *>(interactionPluginNamespace.toStdString());

  if (not workflowInteraction) {
    errormsg = tr("Could not get interface of plugin '%1'.").arg(interactionPluginNamespace);
  } else {
    if (workflow_scene->workflowSaveFile().isEmpty() || workflow_scene->isModified()) {
      QMessageBox::StandardButton reply;
      reply = QMessageBox::question(this, "WorkflowEditor",
                                    tr("Before executing the workflow it must be saved.\n" \
                                       "Do you want to save this workflow now?"),
                                    QMessageBox::Yes | QMessageBox::Cancel);
      if (reply == QMessageBox::Yes) {
        if (not saveWorkflow()) return;
      } else {
        return;
      }
    }

    switchToPlugin(interactionPluginNamespace);

    // set the workflow to execute
    workflowInteraction->setWorkflowFile(workflow_scene->workflowSaveFile());
  }
  if (not errormsg.isEmpty()) {
    errorDialog(tr("Unable to execute workflow"), errormsg);
  }
}

void WorkflowView::switchToExecutionPlugin() {
  switchToPlugin("/plugins/application/workflowexecution");
}

void WorkflowView::setOpacityForNode(NodeId nodeId, qreal opacity, qreal visibility_threshold) {
  QtNodes::NodeGraphicsObject *nodeGraphicsObject = workflow_scene->nodeGraphicsObject(nodeId);
  if (opacity <= visibility_threshold) {
    // make the node disappear completely to make it unusable / unclickable when not visible
    nodeGraphicsObject->setVisible(false);
    QWidget *widget = workflow_scene->getWorkFlowGraphModel().delegateModel<QtNodes::NodeDelegateModel>(nodeId)->embeddedWidget();
    if (widget) {
      widget->clearFocus();
    }
  } else {
    nodeGraphicsObject->setVisible(true);
    QJsonDocument json = QJsonDocument::fromVariant(workflow_scene->getWorkFlowGraphModel().nodeData(nodeId, NodeRole::Style));
    qreal opacityscale = json["NodeStyle"].toObject()["Opacity"].toDouble();
    nodeGraphicsObject->setOpacity(opacity*opacityscale);
    // QWidget *widget = workflow_scene->getWorkFlowGraphModel().delegateModel<QtNodes::NodeDelegateModel>(nodeId)->embeddedWidget();
    // if (widget) {
    //   QColor foreground_color = widget->palette().color(QWidget::foregroundRole());
    //   const QColor &background_color = widget->palette().color(QWidget::backgroundRole());
    //   QString foreground = QString("rgba(%1, %2, %3, %4)")
    //     .arg(foreground_color.red()).arg(foreground_color.green()).arg(foreground_color.blue()).arg(opacity);
    //   QString background = QString("rgba(%1, %2, %3, %4)")
    //     .arg(background_color.red()).arg(background_color.green()).arg(background_color.blue()).arg(opacity);
    //   widget->setStyleSheet(QString("color: %1; background-color: %2").arg(foreground, background));
    //
    //   widget->clearFocus();
    // }
    // QWidget *cornerWidget = node->nodeDataModel()->cornerWidget();
    // if (cornerWidget) {
    //   cornerWidget->setVisible(opacity >= 0.8); // workaround since unable to set transparency for ExecutionProfileChooser
    // }
  }
}

void WorkflowView::applyViewModeOpacity() {
  std::unordered_set<NodeId> affected_nodes;
  qreal opacity = view_mode_opacity / 100.0;
  for (auto nodeId : workflow_scene->getWorkFlowGraphModel().allNodeIds()) {
    if (hideNodeWithViewMode(nodeId)) {
      setOpacityForNode(nodeId, opacity, view_mode_visibility_threshold);
      affected_nodes.insert(nodeId);
    }
  }
  applyViewModeOpacityForConnections(affected_nodes, opacity);
}

void WorkflowView::applyViewModeOpacityForNode(NodeId nodeId) {
  if (hideNodeWithViewMode(nodeId)) {
    QGraphicsOpacityEffect *eff = new QGraphicsOpacityEffect(this);
    QPropertyAnimation *a = new QPropertyAnimation(eff, "opacity");
    a->setDuration(3000);

    qreal opacity = view_mode_opacity / 100.0;

    a->setStartValue(1.0);
    a->setEndValue(opacity);
    a->setEasingCurve(QEasingCurve::OutBack);
    a->start(QPropertyAnimation::DeleteWhenStopped);
    connect(eff, &QGraphicsOpacityEffect::opacityChanged, this, [this, nodeId] (qreal opacity) {
      setOpacityForNode(nodeId, opacity, view_mode_visibility_threshold);
      // applyViewModeOpacityForConnections({nodeId}, opacity);
    });
  }
}

void WorkflowView::applyViewModeOpacityForConnections(const std::unordered_set<NodeId>& affected_nodes, qreal opacity) {
  for (auto connectionId : workflow_scene->getWorkFlowGraphModel().allConnectionIds()) {
    if (affected_nodes.find(connectionId.inNodeId)  != affected_nodes.end() ||
        affected_nodes.find(connectionId.outNodeId) != affected_nodes.end()) {
      setOpacityForConnection(connectionId, opacity, view_mode_visibility_threshold);
    }
  }
}

void WorkflowView::applyViewModeOpacityForConnection(ConnectionId const connectionId) {
  NodeId inNodeId  = connectionId.inNodeId;
  NodeId outNodeId = connectionId.outNodeId;
  if (hideNodeWithViewMode(inNodeId) || hideNodeWithViewMode(outNodeId)) {
    setOpacityForConnection(connectionId, view_mode_opacity / 100.0, view_mode_visibility_threshold);
  } else {
    setOpacityForConnection(connectionId, 1.0);
  }
}

void WorkflowView::setOpacityForConnection(ConnectionId connectionId, qreal opacity, qreal visibility_threshold) {
  QtNodes::ConnectionGraphicsObject *connectionGraphicsObject = workflow_scene->connectionGraphicsObject(connectionId);

  if (opacity <= visibility_threshold) {
    connectionGraphicsObject->setVisible(false);
  } else {
    connectionGraphicsObject->setVisible(true);
    connectionGraphicsObject->setOpacity(opacity);
  }
}

void WorkflowView::resetViewModeOpacity() {
  for (auto nodeId : workflow_scene->getWorkFlowGraphModel().allNodeIds()) {
    setOpacityForNode(nodeId, 1.0);
  }
  for (auto connectionId : workflow_scene->getWorkFlowGraphModel().allConnectionIds()) {
    QtNodes::ConnectionGraphicsObject *connectionGraphicsObject = workflow_scene->connectionGraphicsObject(connectionId);
    connectionGraphicsObject->setVisible(true);
    connectionGraphicsObject->setOpacity(1.0);
  }
}

bool WorkflowView::hideNodeWithViewMode(NodeId nodeId) const {
  QtNodes::NodeDelegateModel *model = workflow_scene->getWorkFlowGraphModel().delegateModel<QtNodes::NodeDelegateModel>(nodeId);
  switch (view_mode) {
    case SHOW_ALL:
      return false;
    case HIDE_INPUTS:
      return dynamic_cast<SourceNode*>(model);
    case HIDE_USER_INPUTS:
      return dynamic_cast<UserInteractionNode*>(model);
    case HIDE_INPUTS_AND_USER_INPUTS:
      return dynamic_cast<SourceNode*>(model) || dynamic_cast<UserInteractionNode*>(model);
  }
  return false;
}

bool WorkflowView::nodeAffectedByViewModes(NodeId nodeId) {
  QtNodes::NodeDelegateModel *model = workflow_scene->getWorkFlowGraphModel().delegateModel<QtNodes::NodeDelegateModel>(nodeId);
  return dynamic_cast<SourceNode*>(model) || dynamic_cast<UserInteractionNode*>(model);
}

void WorkflowView::setViewMode(ViewMode mode) {
  if (view_mode != mode) {
    settingsinterface->setPluginSetting("/plugins/application/workfloweditor", "view_mode", mode);
    view_mode = mode;
    Q_EMIT viewModeChanged(mode);
    resetViewModeOpacity();
    applyViewModeOpacity();
  }
}

void WorkflowView::setViewModeOpacity(int opacity) {
  settingsinterface->setPluginSetting("/plugins/application/workfloweditor", "view_mode_opacity", opacity);
  view_mode_opacity = opacity;
  applyViewModeOpacity();
  Q_EMIT viewModeOpacityChanged(opacity);
}

void WorkflowView::autoLayout() {
  QMessageBox::StandardButton reply;
  reply = QMessageBox::question(this, tr("Apply auto layout?"),
                                tr("Are you sure that you want to automatically align all nodes in the workflow? (experimental)"),
                                QMessageBox::Yes|QMessageBox::No);
  if (reply == QMessageBox::Yes) {
    AutoLayout auto_layout(*workflow_scene);
    auto_layout.apply();
    // since positions may have changed drastically, reset the view to ensure the nodes are visible
    resetView();
  }
}

void WorkflowView::openVariablesDialog() {
  auto dialog_interface = LibFramework::PluginManager::getInstance()->getInterface<EditVariablesDialogInterface*>("/plugins/infrastructure/dialogs/editvariablesdialog");
  dialog_interface->showEditVariablesDialog(workflow_scene->getVariables(), [this] (const QJsonArray& result) {
    workflow_scene->setVariables(result);
  }, [this] (const QJsonArray& dialog_variables) {
    return (workflow_scene->getVariables() != dialog_variables);
  });
}

void WorkflowView::clearWorkflow() {
  if (workflow_scene->isModified()) {
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, tr("New workflow"),
                                  tr("Are you sure that you want to start a new workflow?\n"
                                     "All unsaved changes will be lost."),
                                  QMessageBox::Yes | QMessageBox::Cancel);
    if (reply != QMessageBox::Yes) {
      return;
    }
  }

  workflow_scene->clearScene();

  resetView();
}

void WorkflowView::load(const QString& filepath) {
  try {
    workflow_scene->load(filepath);

    resetView();
    applyViewModeOpacity();
  } catch (std::logic_error& error) {
    errorDialog(tr("Unable to load the Workflow"),
                tr("Unable to load the workflow: %1\n\n%2").arg(filepath, error.what()));
  }
}

void WorkflowView::openLoadDialog(const QString& dialog_namespace) {
  if (checkUnsavedValues()) return;

  PluginChooser pluginchooser(this);
  auto fileDialog = pluginchooser.chooseFrom<FileOpenDialogInterface *>(pluginmanager, dialog_namespace);
  if (!fileDialog) {
    return;
  }
  fileDialog->setFileMode(FileOpenDialogInterface::ExistingFile);
  fileDialog->applyFilter(tr("Workflow Files (*.flow)"));
  if (fileDialog->showFileOpenDialog()) {
    QString path = fileDialog->getFilePath();
    load(path);
  }
}

void WorkflowView::merge(const QString& filepath, const QPointF& offset) {
  try {
    workflow_scene->merge(filepath, offset);

    resetView();
    applyViewModeOpacity();
  } catch (std::logic_error& error) {
    errorDialog(tr("Unable to load the Workflow"),
                tr("Unable to load the workflow: %1\n\n%2").arg(filepath, error.what()));
  }
}

void WorkflowView::upload() {
  KadiIntegrationInterface *kadiintegration = pluginmanager->getInterface<KadiIntegrationInterface*>("/plugins/infrastructure/kadiintegration");
  UploadToKadiDialogInterface *uploadDialog = kadiintegration->createUploadToKadiDialog();
  QString kadiInstanceName;
  QString recordIdentifier;
  QString kadifilename;
  kadiintegration->getRecordIdentifier(workflow_scene->workflowSaveFile(), kadiInstanceName, recordIdentifier, kadifilename);
  if (uploadDialog->showDialog(QJsonDocument(workflow_scene->getWorkFlowGraphModel().save()).toJson(), ".flow", recordIdentifier, kadiInstanceName)) {
    Q_EMIT workflow_scene->filepathChanged(uploadDialog->getFileName());
  }
  delete uploadDialog;
}

bool WorkflowView::save(const QString& filename) {
  try {
    workflow_scene->save(filename);
    return true;
  } catch (std::logic_error& error) {
    errorDialog(tr("Unable to save"), tr("Unable to save the workflow to the given location") + "\n\n"
        + error.what());

    return false;
  }
}

bool WorkflowView::saveWorkflowAs() {
  QString savepath;
  if (workflow_scene->workflowSaveFile().isEmpty()) {
    savepath = QDir::homePath();
  } else {
    savepath = workflow_scene->workflowSaveFile();
  }
  QString filename =
    QFileDialog::getSaveFileName(this,
                                 tr("Save workflow as"),
                                 savepath,
                                 tr("Workflow Files (*.flow)"));

  if (!filename.isEmpty()) {
    if (!filename.endsWith(".flow", Qt::CaseInsensitive)) {
      filename += ".flow";
    }

    return save(filename);
  }

  return false;
}

bool WorkflowView::saveWorkflow() {
  if (QFileInfo::exists(workflow_scene->workflowSaveFile()) && save(workflow_scene->workflowSaveFile())) {
    return true;
  } else {
    return saveWorkflowAs();
  }
}

bool WorkflowView::checkUnsavedValues() {
  if (workflow_scene->isModified()) {
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "WorkflowEditor",
                                  tr("Workflow has changed!\nDo you want to save?"),
                                  QMessageBox::Save | QMessageBox::Ignore | QMessageBox::Cancel);

    switch (reply) {
      case QMessageBox::Save:
        return not saveWorkflow();

      case QMessageBox::Cancel:
        return true;

      default:
        ; // ignore just goes through
    }
  }

  return false;
}

void WorkflowView::adjustDragPositions(QMouseEvent* event, qreal border)
{
  if (border < DIAGRAMSCENE_BORDER_WIDTH)
    border = DIAGRAMSCENE_BORDER_WIDTH;

  const QPointF posF = event->position();      // Qt 6: preferred API
  qreal diffX = 0.0;
  qreal diffY = 0.0;

  qreal mouseX = posF.x();
  qreal mouseY = posF.y();

  // Left border
  if (posF.x() < border) {
    diffX  = std::max(posF.x(), qreal(0)) - border;
    mouseX = border;
  }

  // Right border
  const qreal distX = qreal(this->width()) - border;
  if (posF.x() > distX) {
    diffX  = std::min(posF.x() - distX, border);
    mouseX = distX;
  }

  // Top border
  if (posF.y() < border) {
    diffY  = std::max(posF.y(), qreal(0)) - border;
    mouseY = border;
  }

  // Bottom border
  const qreal distY = qreal(this->height()) - border;
  if (posF.y() > distY) {
    diffY  = std::min(posF.y() - distY, border);
    mouseY = distY;
  }

  if (diffX != 0.0 || diffY != 0.0) {
    // Move the scene
    setSceneRect(sceneRect().translated(diffX, diffY));

    // Reposition the cursor to the clamped point (convert to ints where required)
    this->clearFocus();
    const QPoint localTarget(qRound(mouseX), qRound(mouseY));
    const QPoint globalTarget = this->mapToGlobal(localTarget);
    QCursor::setPos(globalTarget);
    this->setFocus();
  }
}

void WorkflowView::mouseMoveEvent(QMouseEvent* event) {
  GraphicsView::mouseMoveEvent(event);
  if (scene()->mouseGrabberItem() != nullptr && event->buttons() == Qt::LeftButton) {
    adjustDragPositions(event, DIAGRAMSCENE_BORDER_WIDTH);
    // event->accept();
  }
}

void WorkflowView::dragEnterEvent(QDragEnterEvent* e) {
  if (e->mimeData()->hasUrls()) {
    QFileInfo fi(e->mimeData()->urls().at(0).fileName());
    if (fi.suffix() == "flow") {
      // if (e->keyboardModifiers() & Qt::ShiftModifier) {
        e->setDropAction(Qt::MoveAction);
        // e->acceptProposedAction();
        e->accept();
      // } else {
      //   e->setDropAction(Qt::CopyAction);
      //   // e->acceptProposedAction();
      //   e->accept();
      // }
      return;
    }
  }
  e->ignore();
}

void WorkflowView::dragMoveEvent(QDragMoveEvent* e) {
  if (e->mimeData()->hasUrls()) {
    QFileInfo fi(e->mimeData()->urls().at(0).fileName());
    if (fi.suffix() == "flow") {
      if (QApplication::queryKeyboardModifiers() & Qt::ShiftModifier) {
        e->setDropAction(Qt::CopyAction);
        e->acceptProposedAction();
      } else {
        e->setDropAction(Qt::MoveAction);
        e->accept();
      }
      return;
    }
  }
  e->ignore();
}

void WorkflowView::dropEvent(QDropEvent* e)
{
  if (e->mimeData()->hasUrls()) {
    const QString filepath = e->mimeData()->urls().at(0).toLocalFile();
    QFileInfo fi(filepath);
    if (fi.suffix() == QLatin1String("flow")) {
      if (QApplication::queryKeyboardModifiers() & Qt::ShiftModifier) {
        // Qt 6: pos() → position().toPoint()
        const QPointF offset = mapToScene(e->position().toPoint());
        merge(filepath, offset);
      } else {
        if (!checkUnsavedValues())
          load(filepath);
      }
      e->acceptProposedAction();
      return;
    }
  }
  e->ignore();
}

void WorkflowView::errorDialog(const QString& title, const QString& message) {
  qDebug() << title << ": " <<  message;
  QMessageBox::critical(this, title, message);
}
