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

#include "variablejsonnode.h"

QString VariableJsonNode::caption() const {
  return "VariableJson";
}

QString VariableJsonNode::name() const {
  return QString("VariableJson");
}

unsigned int VariableJsonNode::nPorts(PortType portType) const {
  unsigned int result = 0;

  switch (portType) {
    case PortType::In:
      result = 3;
      break;

    case PortType::Out:
      result = 1;

    default:
      break;
  }

  return result;
}

NodeDataType VariableJsonNode::dataType(PortType portType, PortIndex portIndex) const {

  switch (portType) {
    case PortType::In:
      switch (portIndex) {
        case 0:
          return DataTypes::DEPENDENCY;
        case 1:
        case 2:
          return DataTypes::STRING;
        default:
          break;
      }
      break;

    case PortType::Out:
      if (portIndex == 0) {
        return DataTypes::DEPENDENCY;
      }
      break;

    case PortType::None:
    default:
      break;
  }

  return {};
}

QString VariableJsonNode::portCaption(PortType portType, PortIndex portIndex) const {
  switch (portType) {
    case PortType::In:
      switch (portIndex) {
        case 0:
          return "Dependencies";
        case 1:
          return "string: json";
        case 2:
          return "string: key [default: \"data\"]";
        default:
          return unknownPortCaption;
      }

    case PortType::Out:
      if (portIndex == 0) {
        return "Dependents";
      } else {
        return unknownPortCaption;
      }

    case PortType::None:
    default:
      return unknownPortCaption;
  }
}
