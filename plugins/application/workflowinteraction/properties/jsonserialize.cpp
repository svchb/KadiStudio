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

#include "jsonserialize.h"

#include <properties/data/properties.h>

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QStringList>

#include <algorithm>
#include <tuple>

static QJsonArray to_json_array(const std::vector<QJsonObject>& json) {
  return std::accumulate(json.cbegin(), json.cend(), QJsonArray{}, [](QJsonArray array, const QJsonObject& obj) {
    array.append(obj);
    return array;
  });
}

static std::tuple<std::string, QJsonValue> get_type_and_value(const Property* property) {
  // TODO: Should not be dependend on the subclass, should be dependent on the type of the value or on the "json_type" hint.
   if (auto float_property = dynamic_cast<const FloatProperty*>(property)) {
    return {"float", float_property->getValue()};
  } else if (auto double_property = dynamic_cast<const DoubleProperty*>(property)) {
    return {"float", double_property->getValue()};
  } else if (auto int_property = dynamic_cast<const IntProperty*>(property)) {
    return {"int", int_property->getValue()};
  } else if (auto long_property = dynamic_cast<const LongProperty*>(property)) {
    return {"int", static_cast<int>(long_property->getValue())};
  } else if (auto bool_property = dynamic_cast<const BoolProperty*>(property)) {
    return {"bool", bool_property->getValue()};
  } else if (auto ambassador_property = dynamic_cast<const Ambassador*>(property)) {
    return {ambassador_property->getHint()->getEntryOrDefault("json_type", std::string("dict")), QJsonValue{}};
  }

  return {"str", QString::fromStdString(property->toString())};
}

static std::vector<QJsonObject> serialize_ambassador(const Ambassador* ambassador);

static QJsonObject serialize_property(const Property* property) {
  const auto [type, value] = get_type_and_value(property);

  QJsonObject json;
  json["key"] = QString::fromStdString(property->getName());
  json["type"] = QString::fromStdString(type);

  if (type == "list" || type == "dict") {
    json["value"] = to_json_array(serialize_ambassador(dynamic_cast<const Ambassador*>(property)));
  } else {
    json["value"] = value;
  }

  return json;
}

static std::vector<QJsonObject> serialize_ambassador(const Ambassador* ambassador) {
  std::vector<QJsonObject> serialized_properties{};

  for (const auto& property : ambassador->getProperties()) {
    serialized_properties.push_back(serialize_property(property.get()));
  }

  return serialized_properties;
}

std::string model_to_json(const Ambassador* ambassador) {
  return QJsonDocument(to_json_array(serialize_ambassador(ambassador))).toJson(QJsonDocument::Indented).toStdString();
}

std::string property_to_json(const Property* property) {
  return QJsonDocument(serialize_property(property)).toJson(QJsonDocument::Indented).toStdString();
}
