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
#include "plugins/application/workfloweditor/nodes/models/sources/sourcenode.h"

#include "integersourcenode.h"

IntegerSourceNode::IntegerSourceNode() : SourceNode(), lineEdit(new QLineEdit()) {
  setValue(getDefaultValue());
  connect(lineEdit, &QLineEdit::textEdited,
          this, &IntegerSourceNode::onTextEdited);
  lineEdit->setAttribute(Qt::WA_NoSystemBackground);
}

void IntegerSourceNode::onTextEdited(QString const&) {
  value = std::make_unique<GenericNodeData>(DataTypes::INTEGER, lineEdit->text());
  Q_EMIT dataUpdated(0);
}

NodeDataType IntegerSourceNode::dataType(PortType, PortIndex) const {
  return DataTypes::INTEGER;
}

void IntegerSourceNode::load(QJsonObject const& p) {
  QJsonValue v = p["value"];

  if (!v.isUndefined()) {
    bool isInt = false;
    int intValue = v.toVariant().toInt(&isInt);
    QString strValue;
    if (isInt) {
      // convert in case the flow file contains the old value format (v.toString() would then fail)
      strValue = QString::number(intValue);
    } else {
      strValue = v.toString();
    }
    if (strValue.isEmpty()) {
      strValue = getDefaultValue();
    }
    setValue(strValue);
  } else {
    setValue(getDefaultValue());
  }
}

QString IntegerSourceNode::getDefaultValue() const {
  return "0";
}

void IntegerSourceNode::setValue(const QString& _value) {
  auto newValue = std::make_unique<GenericNodeData>(DataTypes::INTEGER, _value);
  lineEdit->setText(_value);
  value = std::move(newValue);
}
