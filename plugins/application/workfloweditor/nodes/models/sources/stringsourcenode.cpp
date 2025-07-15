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

#include "../../data/genericnodedata.h"
#include "../../data/workflowdatatypes.h"
#include "plugins/application/workfloweditor/nodes/models/workflownode.h"

#include "stringsourcenode.h"

StringSourceNode::StringSourceNode() : SourceNode(), lineedit(new QLineEdit()) {
  setValue(getDefaultValue());
  connect(lineedit, &QLineEdit::textEdited,
          this, &StringSourceNode::onTextEdited);
  // lineedit->setAttribute(Qt::WA_NoSystemBackground);
}

void StringSourceNode::onTextEdited(QString const&) {
  value = std::make_unique<GenericNodeData>(DataTypes::STRING, lineedit->text());
  Q_EMIT dataUpdated(0);
}

NodeDataType StringSourceNode::dataType(PortType, PortIndex) const {
  return DataTypes::STRING;
}

void StringSourceNode::load(QJsonObject const& p) {
  QJsonValue v = p["value"];

  if (!v.isUndefined()) {
    QString data = v.toString();
    setValue(data);
  } else {
    setValue(getDefaultValue());
  }
}

QString StringSourceNode::caption() const {
  return "String Source";
}

QString StringSourceNode::name() const {
  return "String";
}

QWidget* StringSourceNode::embeddedWidget() {
  return lineedit;
}

void StringSourceNode::setValue(const QString& value) {
  auto new_value = std::make_unique<GenericNodeData>(DataTypes::STRING, value);
  lineedit->setText(value);
  this->value = std::move(new_value);
}

QString StringSourceNode::getDefaultValue() const {
  return {};
}
