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

#include <nodes/data/workflowdatatypes.h>

#include "userinputintegernode.h"

QString UserInputIntegerNode::name() const {
  return "UserInputInteger";
}

unsigned int UserInputIntegerNode::nPorts(PortType portType) const {
  if (portType == PortType::In) {
    return 3;
  } else {
    return 2;
  }
}

QString UserInputIntegerNode::caption() const {
  return "UserInput: Integer";
}

QString UserInputIntegerNode::portCaption(PortType portType, PortIndex portIndex) const {
  if (portType == PortType::In) {
    if (portIndex == 0) {
      return "Dependencies";
    } else if (portIndex == 1) {
      return "Prompt";
    } else if (portIndex == 2) {
      return "Default";
    }
  } else if (portType == PortType::Out) {
    if (portIndex == 0) {
      return "Dependents";
    } else if (portIndex == 1) {
      return "value";
    }
  }
  return unknownPortCaption;
}

NodeDataType UserInputIntegerNode::dataType(PortType portType, PortIndex portIndex) const {
  if (portType == PortType::In) {
    switch (portIndex) {
      case 0:
        return DataTypes::DEPENDENCY;
      case 1:
        return DataTypes::STRING;
      case 2:
        return DataTypes::INTEGER;
      default:
        break;
    }
  } else if (portType == PortType::Out) {
    switch (portIndex) {
      case 0:
        return DataTypes::DEPENDENCY;
      case 1:
        return DataTypes::INTEGER;
      default:
        break;
    }
  }
  return {};
}
