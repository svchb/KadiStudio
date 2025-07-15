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

#include <QFileInfo>
#include <QtNodes/BasicGraphicsScene>
#include "WorkFlowGraphModel.h"

using QtNodes::BasicGraphicsScene;
using QtNodes::NodeId;

/**
 * @brief      Customized BasicGraphicsScene for the workflow editor
 * @ingroup    workfloweditor
 */
class WorkflowScene : public BasicGraphicsScene {
  Q_OBJECT

  public:

    explicit WorkflowScene(WorkFlowGraphModel &graphModel, QObject* parent = nullptr);

    std::vector<NodeId> selectedNodes() const;

    WorkFlowGraphModel& getWorkFlowGraphModel() {
      return dynamic_cast<WorkFlowGraphModel&>(graphModel());
    }

    void setVariables(const QJsonArray& variables) {
      bool changed = (variables != getWorkFlowGraphModel().getVariables());
      getWorkFlowGraphModel().setVariables(variables);
      if (changed) modified();
    }
    const QJsonArray& getVariables() {
      return getWorkFlowGraphModel().getVariables();
    }

    bool isModified() {
      return ismodified;
    }

    const QString& workflowSaveFile() {
      return workflowsavefile;
    }

  Q_SIGNALS:

    void saveFileSet(bool isSet);
    void filepathChanged(const QString& filepath);

  public Q_SLOTS:

    void clearScene();
    void selectAllNodes();
    void load(const QString& path);
    void merge(const QString& filename, const QPointF& offset);
    void save(const QString& filename);

    void exportAsPng();
    void exportAsSvg();

  private Q_SLOTS:

    void postNodeCreation(NodeId nodeId);
    void modified();

  private:

    /* workflowSaveFile: stores information about the workflow description file (path, name, ...)
     * will be set after loading a workflow and using "save as" from the menu
     */
    QString workflowsavefile;
    bool ismodified;

};
