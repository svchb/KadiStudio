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

#include "useroutputwebviewnode.h"

QString UserOutputWebViewNode::name() const {
  return "UserOutputWebView";
}

unsigned int UserOutputWebViewNode::nPorts(PortType portType) const {
  if (portType == PortType::In) {
    return 3;
  } else if (portType == PortType::Out) {
    return 1;
  }
  return 0;
}

QString UserOutputWebViewNode::caption() const {
  return "UserOutput: Web View";
}

QString UserOutputWebViewNode::portCaption(PortType portType, PortIndex portIndex) const {
  if (portType == PortType::In) {
    switch (portIndex) {
      case 0:
        return "Dependencies";
      case 1:
        return "Description";
      case 2:
        return "Url";
      default:
        break;
    }
  } else if (portType == PortType::Out) {
    if (portIndex == 0) {
      return "Dependents";
    }
  }
  return unknownPortCaption;
}

NodeDataType UserOutputWebViewNode::dataType(PortType portType, PortIndex portIndex) const {
  if (portType == PortType::In) {
    switch (portIndex) {
      case 0:
        return DataTypes::DEPENDENCY;
      case 1:
      case 2:
        return DataTypes::STRING;
      default:
        return {};
    }
  } else if (portType == PortType::Out) {
    if (portIndex == 0) {
      return DataTypes::DEPENDENCY;
    }
  }
  return {};
}
