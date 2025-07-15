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

#include "useroutputtextnode.h"

QString UserOutputTextNode::name() const {
  return "UserOutputText";
}

unsigned int UserOutputTextNode::nPorts(PortType portType) const {
  if (portType == PortType::In) {
    return 2;
  } else if (portType == PortType::Out) {
    return 1;
  }
  return 0;
}

QString UserOutputTextNode::caption() const {
  return "UserOutput: Text";
}

QString UserOutputTextNode::portCaption(PortType portType, PortIndex portIndex) const {
  if (portType == PortType::In) {
    if (portIndex == 0) {
      return "Dependencies";
    } else if (portIndex == 1) {
      return "*Text file";
    }
  } else if (portType == PortType::Out) {
    if (portIndex == 0) {
      return "Dependents";
    }
  }
  return unknownPortCaption;
}

NodeDataType UserOutputTextNode::dataType(PortType portType, PortIndex portIndex) const {
  if (portType == PortType::In) {
    if (portIndex == 0) {
      return DataTypes::DEPENDENCY;
    } else if (portIndex == 1) {
      return DataTypes::STRING;
    }
  } else if (portType == PortType::Out) {
    if (portIndex == 0) {
      return DataTypes::DEPENDENCY;
    }
  }
  return {};
}
