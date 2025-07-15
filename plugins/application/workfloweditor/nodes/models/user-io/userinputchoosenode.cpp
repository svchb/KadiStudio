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

#include <QInputDialog>
#include "plugins/application/workfloweditor/nodes/data/workflowdatatypes.h"

#include "userinputchoosenode.h"

QString UserInputChooseNode::caption() const {
  return "UserInput: Choose";
}

QString UserInputChooseNode::portCaption(PortType portType, PortIndex portIndex) const {
  if (portType == PortType::In) {
    if (portIndex == 0) {
      return "Dependencies";
    } else if (portIndex == 1) {
      return "Prompt";
    } else if (portIndex == 2) {
      return "Default";
    } else {
      return QString("Option %1").arg(portIndex - 2);
    }
  } else if (portType == PortType::Out) {
    if (portIndex == 0) {
      return "Dependents";
    } else if (portIndex == 1) {
      return "Selected";
    } else if (portIndex == 2) {
      return "Value";
    }
  }
  return unknownPortCaption;
}

QString UserInputChooseNode::name() const {
  return "UserInputChoose";
}

unsigned int UserInputChooseNode::nPorts(PortType portType) const {
  if (portType == PortType::In) {
    return numberOfOptions + 3;
  } else {
    return 3;
  }
}

QJsonObject UserInputChooseNode::save() const {
  QJsonObject jsonObject = WorkflowNode::save();
  jsonObject["nOptions"] = (int) numberOfOptions;
  return jsonObject;
}

void UserInputChooseNode::load(QJsonObject const& p) {
  WorkflowNode::load(p);
  QJsonValue nOptions = p["nOptions"];

  if (!nOptions.isUndefined()) {
    numberOfOptions = nOptions.toInt(defaultNumberOfOptions);
  }
}

NodeDataType UserInputChooseNode::dataType(PortType portType, PortIndex portIndex) const {
  if (portType == PortType::Out) {
    if (portIndex == 0) {
      return DataTypes::DEPENDENCY;
    } else if (portIndex == 1) {
      return DataTypes::INTEGER;
    } else if (portIndex == 2) {
      return DataTypes::STRING;
    }
  } else if (portType == PortType::In) {
    if (portIndex == 0) {
      return DataTypes::DEPENDENCY;
    } else if (portIndex == 2) {
      return DataTypes::INTEGER;
    } else {
      return DataTypes::STRING;
    }
  }
  return DataTypes::DEPENDENCY;
}

bool UserInputChooseNode::init() {
  bool ok = false;
  numberOfOptions = QInputDialog::getInt(nullptr, "Node configuration", "Please specify the number of options the user should select from",
                                          (int) numberOfOptions, minOptions, maxOptions, 1, &ok);
  return ok;
}
