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

#include "userinputselectnode.h"

QString UserInputSelectNode::name() const {
  return "UserInputSelect";
}

unsigned int UserInputSelectNode::nPorts(PortType portType) const {
  if (portType == PortType::In) {
    return 5;
  } else if (portType == PortType::Out) {
    return 2;
  }
  return 0;
}

QString UserInputSelectNode::caption() const {
  return "UserInput: Select";
}

QString UserInputSelectNode::portCaption(PortType portType, PortIndex portIndex) const {
  if (portType == PortType::In) {
    if (portIndex == 0) {
      return "Dependencies";
    } else if (portIndex == 1) {
      return "Prompt";
    } else if (portIndex == 2) {
      return "Values";
    } else if (portIndex == 3) {
      return "Default";
    } else if (portIndex == 4) {
      return "Delimiter [default: ,]";
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

NodeDataType UserInputSelectNode::dataType(PortType portType, PortIndex portIndex) const {
  if (portType == PortType::In) {
    switch (portIndex) {
      case 0:
        return DataTypes::DEPENDENCY;
      case 1:
      case 2:
      case 3:
      case 4:
        return DataTypes::STRING;
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
