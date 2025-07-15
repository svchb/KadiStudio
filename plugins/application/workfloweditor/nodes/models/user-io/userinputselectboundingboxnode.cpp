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
#include "userinputselectboundingboxnode.h"

QString UserInputSelectBoundingBoxNode::name() const {
  return "UserInputSelectBoundingBox";
}

unsigned int UserInputSelectBoundingBoxNode::nPorts(PortType portType) const {
  if (portType == PortType::In) {
    return 3;
  } else if (portType == PortType::Out) {
    return 5;
  }
  return 0;
}

QString UserInputSelectBoundingBoxNode::caption() const {
  return "UserInput: Select BoundingBox";
}

QString UserInputSelectBoundingBoxNode::portCaption(PortType portType, PortIndex portIndex) const {
  if (portType == PortType::In) {
    switch (portIndex) {
      case 0:
        return "Dependencies";
      case 1:
        return "Prompt";
      case 2:
        return "Image path";
      default:
        break;
    }
  } else if (portType == PortType::Out) {
    switch (portIndex) {
      case 0:
        return "Dependents";
      case 1:
        return "X";
      case 2:
        return "Y";
      case 3:
        return "Width";
      case 4:
        return "Height";
      default:
        break;
    }
  }
  return unknownPortCaption;
}


NodeDataType UserInputSelectBoundingBoxNode::dataType(PortType portType, PortIndex portIndex) const {
  if (portType == PortType::In) {
    switch (portIndex) {
      case 0:
        return DataTypes::DEPENDENCY;
      case 1:
      case 2:
        return DataTypes::STRING;
      default:
        break;
    }
  } else if (portType == PortType::Out) {
    switch (portIndex) {
      case 0:
        return DataTypes::DEPENDENCY;
      case 1:
      case 2:
      case 3:
      case 4:
        return DataTypes::INTEGER;
      default:
        break;
    }
  }
  return {};
}
