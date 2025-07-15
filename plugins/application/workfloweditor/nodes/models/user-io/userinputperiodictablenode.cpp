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
#include "userinputperiodictablenode.h"

QString UserInputPeriodicTableNode::name() const {
  return "UserInputPeriodicTable";
}

unsigned int UserInputPeriodicTableNode::nPorts(PortType portType) const {
  if (portType == PortType::In) {
    return 3;
  } else {
    return 2;
  }
}

QString UserInputPeriodicTableNode::caption() const {
  return "UserInput: Periodic Table";
}

QString UserInputPeriodicTableNode::portCaption(PortType portType, PortIndex portIndex) const {
  if (portType == PortType::In) {
    switch (portIndex) {
      case 0:
        return "Dependencies";
      case 1:
        return "Prompt";
      case 2:
        return "Default";
      default:
        break;
    }

  } else if (portType == PortType::Out) {
    switch (portIndex) {
      case 0:
        return "Dependents";
      case 1:
        return "Selected elements";
      default:
        break;
    }
  }
  return unknownPortCaption;
}


NodeDataType UserInputPeriodicTableNode::dataType(PortType portType, PortIndex portIndex) const {
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
