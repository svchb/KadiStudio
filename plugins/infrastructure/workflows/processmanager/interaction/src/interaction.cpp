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

#include <QtCore/QJsonObject>
#include <QtCore/QJsonArray>

#include "interaction.h"

std::unique_ptr<InteractionInterface> Interaction::create() const {
  return std::make_unique<Interaction>();
}

void Interaction::fromJson(QJsonObject jsonObj) {
  id = jsonObj["id"].toString();
  typeidentifier = jsonObj["type"].toString();
  pageNumber = jsonObj["pageNumber"].toInt();
  order = jsonObj["order"].toInt();
  prompt = jsonObj["description"].toString();
  default_value = jsonObj["default_value"].toVariant();
  url = jsonObj["url"].toString();
  auto directionStr = jsonObj["direction"].toString();
  if (directionStr == "input") {
    direction = InteractionDirection::INPUT;
  } else if (directionStr == "output") {
    direction = InteractionDirection::OUTPUT;
  }

  if (jsonObj.contains("value") && !jsonObj["value"].isNull()) {
    value = QVariant(jsonObj["value"]);
  }

  if (jsonObj.contains("options") && jsonObj["options"].isArray()) {
    QJsonArray optionsArray = jsonObj["options"].toArray();
    for (auto option : optionsArray) {
      options.push_back(option.toString());
    }
  }

  if (jsonObj.contains("multiline") && jsonObj["multiline"].isBool()) {
    multiline = jsonObj["multiline"].toBool();
  }
}

const QString &Interaction::getPrompt() const {
  return prompt;
}

const QString &Interaction::getTypeIdentifier() const {
  return typeidentifier;
}

const QString &Interaction::getUrl() const {
  return url;
}

const QVariant &Interaction::getDefaultValue() const {
  return default_value;
}

void Interaction::setValue(const QVariant &value) {
  Interaction::value = value;
}

const QVariant &Interaction::getValue() const {
  return value;
}

const QString& Interaction::getId() const {
  return id;
}

int Interaction::getPageNumber() const {
  return pageNumber;
}

InteractionDirection Interaction::getDirection() const {
  return direction;
}

int Interaction::getOrder() const {
  return order;
}

const std::vector<QString> &Interaction::getOptions() const {
  return options;
}

bool Interaction::isMultiline() const {
  return multiline;
}
