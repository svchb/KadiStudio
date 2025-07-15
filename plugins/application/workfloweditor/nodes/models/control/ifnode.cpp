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

#include "nodes/data/workflowdatatypes.h"

#include "ifnode.h"

QString IfNode::caption() const {
  return QString("If Branch");
}

QString IfNode::name() const {
  return QString("IfBranch");
}

unsigned int IfNode::nPorts(PortType portType) const {
  unsigned int result = 0;

  switch (portType) {
    case PortType::In:
      result = 2;
      break;

    case PortType::Out:
      result = 3;

    default:
      break;
  }

  return result;
}

NodeDataType IfNode::dataType(PortType portType, PortIndex portIndex) const {
  switch (portType) {
    case PortType::In:
      switch (portIndex) {
        case 0:
          return DataTypes::DEPENDENCY;
        case 1:
          return DataTypes::BOOLEAN;
        default:
          return {};
      }

    case PortType::Out:
      switch (portIndex) {
        case 0:
        case 1:
        case 2:
          return DataTypes::DEPENDENCY;
        default:
          return {};
      }

    default:
      return {};
  }
}

QString IfNode::portCaption(PortType portType, PortIndex portIndex) const {
  switch (portType) {
    case PortType::In:
      switch (portIndex) {
        case 0:
          return "Dependencies";
        case 1:
          return "condition";
        default:
          return unknownPortCaption;
      }

    case PortType::Out:
      switch (portIndex) {
        case 0:
          return "Dependents";
        case 1:
          return "true";
        case 2:
          return "false";
        default:
          return unknownPortCaption;
      }
    default:
      return unknownPortCaption;
  }
}
