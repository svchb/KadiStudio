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

#include "nodes/data/genericnodedata.h"
#include "nodes/data/workflowdatatypes.h"

#include "floatsourcenode.h"

FloatSourceNode::FloatSourceNode() : SourceNode(), lineEdit(new QLineEdit()) {
  setValue(getDefaultValue());
  connect(lineEdit, &QLineEdit::textEdited,
          this, &FloatSourceNode::onTextEdited);
  lineEdit->setAttribute(Qt::WA_NoSystemBackground);
}

void FloatSourceNode:: onTextEdited(QString const&) {
  value = std::make_unique<GenericNodeData>(DataTypes::FLOAT, lineEdit->text());
  Q_EMIT dataUpdated(0);
}

NodeDataType FloatSourceNode:: dataType(PortType, PortIndex) const {
  return DataTypes::FLOAT;
}

void FloatSourceNode::load(QJsonObject const& p) {
  QJsonValue v = p["value"];

  if (!v.isUndefined()) {
    bool isFloat = false;
    float floatValue = v.toVariant().toFloat(&isFloat);
    QString strValue;
    if (isFloat) {
      // convert in case the flow file contains the old value format (v.toString() would then fail)
      strValue = QString::number(floatValue);
    } else {
      strValue = v.toString();
    }
    if (strValue.isEmpty()) {
      strValue = getDefaultValue();
    }
    setValue(strValue);
    lineEdit->setText(strValue);
  } else {
    setValue(getDefaultValue());
  }
}

void FloatSourceNode::setValue(const QString& _value) {
  auto newValue = std::make_unique<GenericNodeData>(DataTypes::FLOAT, _value);
  lineEdit->setText(_value);
  value = std::move(newValue);
}

QString FloatSourceNode::getDefaultValue() const {
  return "0.0";
}
