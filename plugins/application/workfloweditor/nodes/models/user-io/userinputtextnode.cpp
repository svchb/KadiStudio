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

#include "userinputtextnode.h"

QString UserInputTextNode::name() const {
  return "UserInputText";
}

unsigned int UserInputTextNode::nPorts(PortType portType) const {
  if (portType == PortType::In) {
    return 4;
  } else {
    return 2;
  }
}

QString UserInputTextNode::caption() const {
  return "UserInput: Text";
}

QString UserInputTextNode::portCaption(PortType portType, PortIndex portIndex) const {
  if (portType == PortType::In) {
    if (portIndex == 0) {
      return "Dependencies";
    } else if (portIndex == 1) {
      return "Prompt";
    } else if (portIndex == 2) {
      return "Default";
    } else if (portIndex == 3) {
      return "Multiline";
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

NodeDataType UserInputTextNode::dataType(PortType portType, PortIndex portIndex) const {
  if (portType == PortType::In) {
    switch (portIndex) {
      case 0:
        return DataTypes::DEPENDENCY;
      case 1:
      case 2:
        return DataTypes::STRING;
      case 3:
        return DataTypes::BOOLEAN;
      default:
        break;
    }
  } else if (portType == PortType::Out) {
    switch (portIndex) {
      case 0:
        return DataTypes::DEPENDENCY;
      case 1:
        return DataTypes::STRING;
      default:
        break;
    }
  }
  return {};
}
