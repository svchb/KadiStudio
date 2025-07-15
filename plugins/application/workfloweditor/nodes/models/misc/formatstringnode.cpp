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
#include "nodes/data/genericnodedata.h"
#include "nodes/data/workflowdatatypes.h"

#include "formatstringnode.h"

FormatStringNode::FormatStringNode() : lineedit(new QLineEdit()) {
  lineedit->setAttribute(Qt::WA_NoSystemBackground);
}

NodeDataType FormatStringNode::dataType(PortType, PortIndex portindex) const {
  if (portindex == 0) {
    return DataTypes::DEPENDENCY;
  }
  return DataTypes::STRING;
}

QJsonObject FormatStringNode::save() const {
  QJsonObject jsonObject = WorkflowNode::save();
  jsonObject["value"] = lineedit->text();
  jsonObject["nInputs"] = (int) numberofinputs;
  return jsonObject;
}

void FormatStringNode::load(QJsonObject const& p) {
  WorkflowNode::load(p);

  QJsonValue inputs = p["nInputs"];
  if (!inputs.isUndefined()) {
    numberofinputs = std::max(0, inputs.toInt(defaultnumberofinputs));
  }

  QJsonValue v = p["value"];
  if (!v.isUndefined()) {
    lineedit->setText(v.toString());
  } else {
    lineedit->setText(generateDefaultValue());
  }
}

QString FormatStringNode::caption() const {
  return "Format String";
}

unsigned int FormatStringNode::nPorts(PortType porttype) const {
  if (porttype == QtNodes::PortType::In) {
    return numberofinputs + 1;
  } else {
    return 2;
  }
}

QString FormatStringNode::portCaption(PortType porttype, PortIndex portindex) const {
  if (porttype == QtNodes::PortType::In) {
    if (portindex == 0) {
      return "Dependencies";
    } else {
      return QString("%%1").arg(portindex - 1);
    }
  } else if (porttype == QtNodes::PortType::Out) {
    if (portindex == 0) {
      return "Dependents";
    } else if (portindex == 1) {
      return "formatted string";
    }
  }
  return {};
}

QString FormatStringNode::name() const {
  return "FormatString";
}

QWidget *FormatStringNode::embeddedWidget() {
  return lineedit;
}

QString FormatStringNode::generateDefaultValue() const {
  // generating a reasonable default, in the form of "[%0, %1, ...]"
  QString result = "[";
  for (unsigned int i = 0; i < numberofinputs; i++) {
    result += QString("%%1").arg(i);
    if (i + 1 < numberofinputs) {
      result += ", ";
    }
  }
  return result += "]";
}

bool FormatStringNode::init() {
  bool ok = false;
  numberofinputs = QInputDialog::getInt(nullptr, tr("Node configuration"), tr("Please specify the number of inputs for the format string"),
                                        (int) defaultnumberofinputs, mininputs, maxinputs, 1, &ok);
  lineedit->setText(FormatStringNode::generateDefaultValue());
  return ok;
}
