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

#include <QPointF>
#include <unordered_map>

#include <QtNodes/Definitions>

#include "workflowscene.h"

using QtNodes::NodeId;

struct ColumnInfo {
  qreal x = 0;
  qreal width = 0;
  qreal next_y = 0;
};

/**
 * @brief      A helper class which can apply automatic layout to nodes in a FlowScene.
 * @ingroup    workfloweditor
 */
class AutoLayout {

public:
  explicit AutoLayout(WorkflowScene& scene);
  void apply();

private:
  void assignNodesToColumns();
  void calculateColumnInfos();
  void processNode(NodeId nodeId, int depth = 0);
  bool isStartNode(NodeId nodeId);

  // setting for the vertical and horizontal distance between nodes in the autolayout
  const QPointF node_distance = {50., 5.};

  WorkflowScene &scene;
  std::unordered_map<NodeId, int> node_columns;
  std::unordered_map<int, ColumnInfo> column_infos;
};
