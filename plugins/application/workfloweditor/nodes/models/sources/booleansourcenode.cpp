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

#include <QVariant>
#include "nodes/data/workflowdatatypes.h"

#include "booleansourcenode.h"

BooleanSourceNode::BooleanSourceNode() : SourceNode(), checkBox(new QCheckBox()) {
  setValue(getDefaultValue());
  connect(checkBox, &QCheckBox::stateChanged,
          this, &BooleanSourceNode::onStateChanged);
  checkBox->setAttribute(Qt::WA_NoSystemBackground);
}

QString BooleanSourceNode::caption() const {
  return QString("Boolean Source");
}

NodeDataType BooleanSourceNode::dataType(PortType, PortIndex) const {
  return DataTypes::BOOLEAN;
}

QWidget *BooleanSourceNode::embeddedWidget() {
  return checkBox;
}

void BooleanSourceNode::onStateChanged(bool) {
  value = std::make_unique<GenericNodeData>(DataTypes::BOOLEAN, stringFromValue(checkBox->isChecked()));
  Q_EMIT dataUpdated(0);
}

void BooleanSourceNode::load(QJsonObject const& p) {
  QJsonValue v = p["value"];

  if (!v.isUndefined()) {
    bool boolValue = false;
    if (v.isBool()) {
      // convert in case the flow file contains the old value format (v.toString() would then fail)
      boolValue = v.toBool();
    } else {
      boolValue = v.toVariant().toBool();
    }
    value = std::make_unique<GenericNodeData>(DataTypes::BOOLEAN, stringFromValue(boolValue));
    checkBox->setChecked(boolValue);
  } else {
    setValue(getDefaultValue());
  }
}

QString BooleanSourceNode::getDefaultValue() const {
  return stringFromValue(false);
}

void BooleanSourceNode::setValue(const QString& _value) {
  auto newValue = std::make_unique<GenericNodeData>(DataTypes::BOOLEAN, _value);
  checkBox->setChecked(valueFromString(_value));
  value = std::move(newValue);
}

QString BooleanSourceNode::stringFromValue(bool value) {
  return QString::number(value);
}

bool BooleanSourceNode::valueFromString(const QString& string) {
  return string == stringFromValue(true);
}
