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

#include "userinputformnode.h"
#include "nodes/data/workflowdatatypes.h"

QString UserInputFormNode::name() const {
  return "UserInputForm";
}

unsigned int UserInputFormNode::nPorts(PortType /*portType*/) const {
  return 2;
}

QString UserInputFormNode::caption() const {
  return "UserInput: Form/Template";
}

QString UserInputFormNode::portCaption(PortType portType, PortIndex portIndex) const {
  if (portType == PortType::In) {
    if (portIndex == 0) {
      return "Dependencies";
    } else if (portIndex == 1) {
      return "Json file";
    }
  } else if (portType == PortType::Out) {
    if (portIndex == 0) {
      return "Dependents";
    } else if (portIndex == 1) {
      return "Json string";
    }
  }
  return unknownPortCaption;
}

NodeDataType UserInputFormNode::dataType(PortType /*portType*/, PortIndex portIndex) const {
  switch (portIndex) {
    case 0:
      return DataTypes::DEPENDENCY;
    case 1:
      return DataTypes::STRING;
    default:
      break;
  }
  return {};
}
