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

#pragma once

#include <QtCore/QJsonDocument>

#include <properties/data/property.h>
#include <properties/data/propertiesmodel.h>
#include <plugins/infrastructure/workflows/processmanager/interaction/interactioninterface.h>

class PropertyParser {

public:
  static QString valueFromProperty(const Property* property);
  static Property* buildProperty(InteractionInterface* interaction);
  static bool validatePropertyValues(const Ambassador* ambassador, std::vector<std::string>& invalid_values);

private:
  static bool validatePropertyValues(const Ambassador* ambassador, std::vector<std::string>& invalid_values, int& index);
  static Ambassador* fromKadiTemplateJson(const std::string& propertyname, const QJsonObject& json_object);

  // recursive parsing function to generate properties from keys of a kadi template json description
  static void parsePropertyArray(PropertiesModel& target, const QJsonArray& json_array);
  static QJsonDocument loadJson(const QString& path);

  static const std::string& getString(const std::string& value, const std::string& fallback_value);
};
