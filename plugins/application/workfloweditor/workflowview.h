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

#include <QGraphicsView>

#include <QtNodes/BasicGraphicsScene>
#include <QtNodes/GraphicsView>
#include <framework/pluginframework/pluginmanagerinterface.h>
#include <unordered_set>

using QtNodes::BasicGraphicsScene;
using QtNodes::GraphicsView;
using QtNodes::NodeId;
using QtNodes::ConnectionId;

class WorkflowScene;
class SettingsInterface;

enum ViewMode {
  SHOW_ALL,
  HIDE_INPUTS,
  HIDE_USER_INPUTS,
  HIDE_INPUTS_AND_USER_INPUTS
};


/**
 * @class      Customized FlowView for the workflow editor
 * @ingroup    workfloweditor
 */
class WorkflowView : public GraphicsView {
  Q_OBJECT

  public:
    explicit WorkflowView(BasicGraphicsScene* scene = nullptr, QWidget* parent = nullptr,
                          LibFramework::PluginManagerInterface* pluginmanager = nullptr);
    WorkflowView(const GraphicsView&) = delete;

    void loadSettings();
    SettingsInterface* getSettings() {
      return settingsinterface;
    }

    #define DIAGRAMSCENE_BORDER_WIDTH 30

    void mouseMoveEvent(QMouseEvent* event) Q_DECL_OVERRIDE;

    void dragEnterEvent(QDragEnterEvent* e) Q_DECL_OVERRIDE;
    void dragMoveEvent(QDragMoveEvent* e) Q_DECL_OVERRIDE;
    void dropEvent(QDropEvent* e) Q_DECL_OVERRIDE;

    /*
     * A category name for NodeModels which should not be listed in the context menu for adding new nodes.
     * This is necessary for ToolNodes and EnvNodes, because they are built and added programmatically via the "Add Tool"
     * entry. The actual string value does not matter, because it is never displayed anywhere.
     */
    const static QString EXCLUDED_CATEGORY;

    static const int RESET_VIEW_MIN_WIDTH = 600;
    static const int RESET_VIEW_MIN_HEIGHT = 600;

  public Q_SLOTS:
    void showGrid(bool isChecked);
    void startExecution();
    void switchToExecutionPlugin();
    void applyViewModeOpacity();
    void applyViewModeOpacityForNode(NodeId nodeId);
    void resetViewModeOpacity();
    void setViewMode(ViewMode mode);
    void setViewModeOpacity(int opacity);
    void autoLayout();
    void resetView();
    void switchToPlugin(const QString& pluginname); // TODO in pluginmanager/framework

    void openVariablesDialog();

    void clearWorkflow();
    void openLoadDialog(const QString& dialog_namespace);
    bool checkUnsavedValues();
    void load(const QString& path);
    void merge(const QString& filename, const QPointF& offset);
    bool save(const QString& filename);
    bool saveWorkflowAs();
    bool saveWorkflow();
    void upload();

  Q_SIGNALS:
    void viewModeChanged(ViewMode mode);
    void gridVisibilityChanged(bool visible);
    void viewModeOpacityChanged(int opacity);

  private:
    void adjustDragPositions(QMouseEvent* event, qreal border = DIAGRAMSCENE_BORDER_WIDTH);
    void contextMenuEvent(QContextMenuEvent *event) override;
    void drawBackground(QPainter* painter, const QRectF& r) override;
    void resizeEvent(QResizeEvent* event) override;
    void setOpacityForNode(NodeId nodeId, qreal opacity, qreal visibility_threshold = 0.0);
    void setOpacityForConnection(ConnectionId connectionId, qreal opacity, qreal visibility_threshold = 0.0);
    void applyViewModeOpacityForConnections(const std::unordered_set<NodeId> &affected_nodes, qreal opacity);
    void applyViewModeOpacityForConnection(ConnectionId const connectionId);
    bool hideNodeWithViewMode(NodeId nodeId) const;
    bool nodeAffectedByViewModes(NodeId nodeId);
    void errorDialog(const QString& title, const QString& message);

    LibFramework::PluginManagerInterface *pluginmanager;
    SettingsInterface *settingsinterface;

    WorkflowScene *workflow_scene;
    bool grid_visible;

    QAction *selectallAction;

    ViewMode view_mode;
    int view_mode_opacity;
    const qreal view_mode_visibility_threshold = 0.01;
};
