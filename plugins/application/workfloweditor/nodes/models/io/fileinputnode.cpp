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

#include "fileinputnode.h"

QString FileInputNode::caption() const {
  return "File Input";
}

QString FileInputNode::name() const {
  return QString("FileInput");
}

unsigned int FileInputNode::nPorts(PortType portType) const {
  unsigned int result = 1;

  switch (portType) {
    case PortType::In:
    case PortType::Out:
      result = 2;
      break;
    case PortType::None:
      break;
  }

  return result;
}

NodeDataType FileInputNode::dataType(PortType portType,
                                     PortIndex portIndex) const {

  switch (portType) {
    case PortType::In:
      switch (portIndex) {
        case 0:
          return DataTypes::DEPENDENCY;
        case 1:
          return DataTypes::STRING;
        case 2:
          return DataTypes::PIPE;
        default:
          break;
      }
      break;

    case PortType::Out:
      switch (portIndex) {
        case 0:
          return DataTypes::DEPENDENCY;
        case 1:
          return DataTypes::PIPE;
        default:
          break;
      }

    case PortType::None:
      break;
  }

  return {};
}

QString FileInputNode::portCaption(PortType portType, PortIndex portIndex) const {
  switch (portType) {
    case PortType::In:
      switch (portIndex) {
        case 0:
          return "Dependencies";
        case 1:
          return "File path";
        default:
          return unknownPortCaption;
      }

    case PortType::Out:
      switch (portIndex) {
        case 0:
          return "Dependents";
        case 1:
          return "stdout";
        default:
          return unknownPortCaption;
      }

    default:
      return unknownPortCaption;
  }
}

ConnectionPolicy FileInputNode::portConnectionPolicy(PortType portType, PortIndex portIndex) const {
  if (portType == QtNodes::PortType::Out && portIndex == 1) {
    return ConnectionPolicy::One;
  }
  return NodeDelegateModel::portConnectionPolicy(portType, portIndex);
}
