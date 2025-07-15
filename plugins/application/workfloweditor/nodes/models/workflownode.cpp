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

#include "workflownode.h"

WorkflowNode::WorkflowNode()/* : NodeDelegateModel()*/ {
}

void WorkflowNode::setInData(std::shared_ptr<NodeData>, PortIndex) {
  // implementation would only be necessary if we wanted to process data 'live' in the editor
}

std::shared_ptr<NodeData> WorkflowNode::outData(PortIndex) {
  // implementation would only be necessary if we wanted to process data 'live' in the editor
  return std::shared_ptr<NodeData>();
}

ConnectionPolicy WorkflowNode::portConnectionPolicy(PortType portType, PortIndex portIndex) const {
  return portType == PortType::In && portIndex == 0 ? ConnectionPolicy::Many
                                                    : NodeDelegateModel::portConnectionPolicy(portType, portIndex);
}

QWidget *WorkflowNode::embeddedWidget() {
  return nullptr;
}

QJsonObject WorkflowNode::save() const {
  // implementation is only necessary when custom data needs to be saved to the .flow file
  // (see for example SourceNodes, FileOutputNode, BranchSelectNode, UserInputChooseNode, ToolNode)
  QJsonObject modelJson;

  modelJson["name"] = name();

  return modelJson;
  // return NodeDelegateModel::save(); // TODO Bug in QtNodes load ist in Model aber save in NodeDelegateModel
}

void WorkflowNode::load(const QJsonObject& p) {
  // implementation is only necessary when custom data needs to be restored (see for example SourceNodes,
  // FileOutputNode, BranchSelectNode, UserInputChooseNode, ToolNode)
  NodeDelegateModel::load(p);
}

QString WorkflowNode::portCaption(PortType portType, PortIndex portIndex) const {
  return dataType(portType, portIndex).name;
}
