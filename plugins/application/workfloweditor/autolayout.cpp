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

#include "workflowscene.h"
#include "WorkFlowGraphModel.h"
#include <QtNodes/internal/NodeGraphicsObject.hpp>
#include <QtNodes/internal/ConnectionGraphicsObject.hpp>

#include "nodes/models/sources/sourcenode.h"
#include "autolayout.h"


AutoLayout::AutoLayout(WorkflowScene &scene) : scene(scene) {
}

void AutoLayout::assignNodesToColumns() {
  node_columns.clear();

  std::vector<NodeId> start_nodes, source_nodes;
  for (auto nodeId : scene.getWorkFlowGraphModel().allNodeIds()) {
    if (scene.getWorkFlowGraphModel().delegateModel<SourceNode>(nodeId)) {
      source_nodes.push_back(nodeId);
      continue;
    }
    if (isStartNode(nodeId)) {
      start_nodes.push_back(nodeId);
    }
  }

  // recursively process the start nodes, using a depth-first search like approach
  for (NodeId start_node: start_nodes) {
    processNode(start_node);
  }

  // source nodes have been ignored so far, they should be in the column _before_ the node(s) which use their value
  for (NodeId source_node : source_nodes) {
    int minimum_column = 0;
    bool found_one = false;
    for (const auto& outgoing_ports : scene.getWorkFlowGraphModel().connections(source_node, PortType::Out)) {
      NodeId other_node = outgoing_ports.inNodeId;
      if (!found_one || node_columns[other_node] < minimum_column) {
        minimum_column = node_columns[other_node];
        found_one = true;
      }
    }
    node_columns[source_node] = minimum_column - 1;
  }
}

void AutoLayout::processNode(NodeId nodeId, int depth) {
  if (node_columns[nodeId] != 0 && node_columns[nodeId] >= depth) {
    // stop recursion, this node was visited already
    return;
  }

  node_columns[nodeId] = depth;

  // follow all outgoing connections
  for (const auto& outgoing_ports : scene.getWorkFlowGraphModel().connections(nodeId, PortType::Out)) {
    processNode(outgoing_ports.inNodeId, depth + 1);
  }
}

void AutoLayout::calculateColumnInfos() {
  column_infos.clear();

  // prepare the column info structs first, otherwise the column width changes while repositioning the nodes
  int max_column_id = 0, min_column_id = 0;
  for (const auto& node_column_entry : node_columns) {
    const NodeId nodeId = node_column_entry.first;
    const int column_id = node_column_entry.second;

    ColumnInfo &column_info = column_infos[column_id];

    const QRectF &node_rect = scene.nodeGraphicsObject(nodeId)->boundingRect();
    column_info.width = std::max(column_info.width, node_rect.width());

    max_column_id = std::max(max_column_id, column_id);
    min_column_id = std::min(min_column_id, column_id);
  }

  // the width of each column is now determined, allowing to set the x values consecutively
  for (int column_id = min_column_id; column_id <= max_column_id; column_id++) {
    ColumnInfo &last_column_info = column_infos[column_id-1];
    ColumnInfo &column_info = column_infos[column_id];
    column_info.x = last_column_info.x + last_column_info.width + node_distance.x();
  }
}

void AutoLayout::apply() {
  MoveNodesCommand *movenodescommand = new MoveNodesCommand(&scene);

  assignNodesToColumns();
  calculateColumnInfos();

  for (const auto& node_column_entry : node_columns) {
    NodeId nodeId = node_column_entry.first;
    const int column_id = node_column_entry.second;

    ColumnInfo &column_info = column_infos[column_id];
    QPointF new_pos = { column_info.x, column_info.next_y };

    movenodescommand->addNodePos(nodeId, new_pos);

    const QRectF &node_rect = scene.nodeGraphicsObject(nodeId)->boundingRect();
    column_info.next_y += node_rect.height() + node_distance.y();
  }

  scene.undoStack().push(movenodescommand);
}

bool AutoLayout::isStartNode(NodeId nodeId) {
  // we have to check if the connection maps for ingoing ports are all empty
  // if not empty, it can still be that all ingoing connections are source nodes, in this case it doesn't count
  for (const auto& port_connections : scene.getWorkFlowGraphModel().connections(nodeId, PortType::In)) {
    NodeId other_node = port_connections.inNodeId;
    if (/*other_node &&*/ ! scene.getWorkFlowGraphModel().delegateModel<SourceNode>(other_node)) {
      return false;
    }
  }
  return true;
}
