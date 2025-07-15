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

#include "fileoutputnode.h"

FileOutputNode::FileOutputNode() : createShortcut(false), shortcutCheckbox(new QCheckBox("shortcut")) {
  connect(shortcutCheckbox, &QCheckBox::stateChanged,
          this, &FileOutputNode::onShortcutStateChanged);
  shortcutCheckbox->setAttribute(Qt::WA_NoSystemBackground);
}

QString FileOutputNode::caption() const {
  return "File Output";
}

QString FileOutputNode::name() const {
  return QString("FileOutput");
}

unsigned int FileOutputNode::nPorts(PortType portType) const {
  unsigned int result = 0;

  switch (portType) {
    case PortType::In:
      result = 4;
      break;

    case PortType::Out:
      result = 1;

    default:
      break;
  }

  return result;
}

NodeDataType FileOutputNode::dataType(PortType portType, PortIndex portIndex) const {
  switch (portType) {
    case PortType::In:
      switch (portIndex) {
        case 0:
          return DataTypes::DEPENDENCY;
        case 1:
          return DataTypes::STRING;
        case 2:
          return DataTypes::BOOLEAN;
        case 3:
          return DataTypes::PIPE;
        default:
          break;
      }
      break;

    case PortType::Out:
      if (portIndex == 0) {
        return DataTypes::DEPENDENCY;
      }
      break;

    case PortType::None:
    default:
      break;
  }

  return {};
}

QWidget *FileOutputNode::embeddedWidget() {
  return shortcutCheckbox;
}

QJsonObject FileOutputNode::save() const {
  QJsonObject jsonObject = WorkflowNode::save();
  jsonObject["createShortcut"] = createShortcut;
  return jsonObject;
}

void FileOutputNode::load(QJsonObject const& p) {
  WorkflowNode::load(p);
  QJsonValue createShortcutJson = p["createShortcut"];
  if (!createShortcutJson.isUndefined()) {
    createShortcut = createShortcutJson.toBool(false);
  }
  shortcutCheckbox->setChecked(createShortcut);
}

QString FileOutputNode::portCaption(PortType portType, PortIndex portIndex) const {
  switch (portType) {
    case PortType::In:
      switch (portIndex) {
        case 0:
          return "Dependencies";
        case 1:
          return "File path";
        case 2:
          return "Append";
        case 3:
          return "stdin";
        default:
          break;
      }
      break;

    case PortType::Out:
      if (portIndex == 0) {
        return "Dependents";
      }
      break;

    case PortType::None:
      break;
  }

  return unknownPortCaption;
}

void FileOutputNode::onShortcutStateChanged(bool newState) {
  createShortcut = newState;
}

ConnectionPolicy FileOutputNode::portConnectionPolicy(PortType portType, PortIndex portIndex) const {
  if (portType == PortType::Out && portIndex == 1) {
    return ConnectionPolicy::One;
  } else if (portType == PortType::In && portIndex == 0) {
    return ConnectionPolicy::Many;
  } else {
    return NodeDelegateModel::portConnectionPolicy(portType, portIndex);
  }
}
