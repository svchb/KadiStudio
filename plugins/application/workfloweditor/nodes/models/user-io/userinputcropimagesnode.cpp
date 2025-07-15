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
#include "userinputcropimagesnode.h"

QString UserInputCropImagesNode::caption() const {
  return "UserInput: Crop Images";
}

QString UserInputCropImagesNode::portCaption(PortType portType, PortIndex portIndex) const {
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
        return "Crop info";
      default:
        break;
    }
  }
  return unknownPortCaption;
}

QString UserInputCropImagesNode::name() const {
  return "UserInputCropImages";
}

unsigned int UserInputCropImagesNode::nPorts(PortType portType) const {
  if (portType == PortType::In) {
    return 3;
  } else if (portType == PortType::Out) {
    return 2;
  }
  return 0;
}

NodeDataType UserInputCropImagesNode::dataType(PortType portType, PortIndex portIndex) const {
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
        return DataTypes::STRING;
      default:
        break;
    }
  }
  return {};
}
