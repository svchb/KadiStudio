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

#include <QInputDialog>

#include "nodes/data/workflowdatatypes.h"

#include "branchselectnode.h"

BranchSelectNode::BranchSelectNode() : WorkflowNode(), numberOfBranches(defaultNumberOfBranches) {
}

QString BranchSelectNode::caption() const {
  return name();
}

unsigned int BranchSelectNode::nPorts(PortType portType) const {
  if (portType == QtNodes::PortType::Out) {
    return numberOfBranches + 1;
  } else {
    return 2;
  }
}

QString BranchSelectNode::portCaption(PortType portType, PortIndex portIndex) const {
  if (portType == QtNodes::PortType::In) {
    if (portIndex == 0) {
      return "Dependencies";
    } else if (portIndex == 1) {
      return "Selected";
    }
  } else if (portType == QtNodes::PortType::Out) {
    if (portIndex == 0) {
      return "Dependents";
    } else {
      return QString("Branch %1").arg(portIndex);
    }
  }
  return {};
}

NodeDataType BranchSelectNode::dataType(PortType portType, PortIndex portIndex) const {
  if (portIndex == 0 || portType == PortType::Out) {
    return DataTypes::DEPENDENCY;
  } else if (portIndex == 1 || portType == PortType::In) {
    return DataTypes::INTEGER;
  }
  return {};
}

QJsonObject BranchSelectNode::save() const {
  QJsonObject jsonObject = WorkflowNode::save();
  jsonObject["nBranches"] = (int) numberOfBranches;
  return jsonObject;
}

void BranchSelectNode::load(const QJsonObject& p) {
  QJsonValue nBranches = p["nBranches"];

  if (!nBranches.isUndefined()) {
    numberOfBranches = nBranches.toInt(defaultNumberOfBranches);
  }
}

QString BranchSelectNode::name() const {
  return "BranchSelect";
}

bool BranchSelectNode::init() {
  bool ok = false;
  numberOfBranches = QInputDialog::getInt(nullptr, "Node configuration", "Please specify the number of branches",
                                          (int) defaultNumberOfBranches, minBranches, maxBranches, 1, &ok);
  return ok;
}
