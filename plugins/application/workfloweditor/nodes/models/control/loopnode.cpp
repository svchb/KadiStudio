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

#include "loopnode.h"


QString LoopNode::caption() const {
  return QString("Loop");
}

QString LoopNode::name() const {
  return QString("Loop");
}

unsigned int LoopNode::nPorts(PortType portType) const {
  unsigned int result = 0;

  switch (portType) {
    case PortType::In:
      result = 6;
      break;

    case PortType::Out:
      result = 3;
      break;

    default:
      break;
  }

  return result;
}

NodeDataType LoopNode::dataType(PortType portType, PortIndex portIndex) const {
  switch (portType) {
    case PortType::In:
      switch (portIndex) {
        case 0:
          return DataTypes::DEPENDENCY;
        case 1:
          return DataTypes::BOOLEAN;
        case 2:
        case 3:
        case 4:
          return DataTypes::INTEGER;
        case 5:
          return DataTypes::STRING;
        default:
          break;
      }
      break;

    case PortType::Out:
      switch (portIndex) {
        case 0:
        case 1:
          return DataTypes::DEPENDENCY;
        case 2:
          return DataTypes::INTEGER;
        default:
          break;
      }
      break;

    case PortType::None:
      break;
  }

  return {};
}

QString LoopNode::portCaption(PortType portType, PortIndex portIndex) const {
  switch (portType) {
    case PortType::In:
      switch (portIndex) {
        case 0:
          return "Dependencies";
        case 1:
          return "Condition";
        case 2:
          return "Start Index [0]";
        case 3:
          return "End Index";
        case 4:
          return "Step [1]";
        case 5:
          return "Index variable name";
        default:
          return unknownPortCaption;
      }

    case PortType::Out:
      switch (portIndex) {
        case 0:
          return "Dependents";
        case 1:
          return "Loop";
        case 2:
          return "Index";
        default:
          return unknownPortCaption;
      }

    default:
      return unknownPortCaption;
  }
}
