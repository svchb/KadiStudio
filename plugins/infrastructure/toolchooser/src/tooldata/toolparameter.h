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

#include <QtCore/QString>
#include <QtCore/QJsonObject>
#include <QtXml/QDomElement>

class ToolParameter {

  public:
    ToolParameter() { clear(); };

    explicit ToolParameter(const QJsonObject& jsonObject) {
      longName = jsonObject["name"].toString();
      name = jsonObject["shortName"].toString();
      type = jsonObject["type"].toString();
      value = jsonObject["value"].toString();
      required = jsonObject["required"].toBool();
      if (jsonObject.contains("positional")) {
        positional = jsonObject["positional"].toBool(false);
      } else {
        positional = false;
      }
    }

    explicit ToolParameter(const QDomElement& e) {
      setLongName(e.attribute("name", ""));
      setName(e.attribute("char", ""));
      setRequired(!e.attribute("required", "").isEmpty());
      setRelations(e.attribute("relations", ""));
      setInterval(e.attribute("interval", ""));
      setDescription(e.attribute("description", ""));
      setType(e.attribute("type", ""));
      setDefaultValue(e.attribute("default", ""));
      setValue(e.attribute("value", ""));
      setEnabled(e.attribute("enabled", "false"));
      bool positional = e.attribute("positional", "false") == "true";
      setPositional(positional);
    }

    bool operator==(const ToolParameter &rhs) const {
      return longName == rhs.longName &&
             name == rhs.name &&
             required == rhs.required &&
             relations == rhs.relations &&
             interval == rhs.interval &&
             description == rhs.description &&
             type == rhs.type &&
             default_value == rhs.default_value &&
             value == rhs.value &&
             enabled == rhs.enabled &&
             positional == rhs.positional;
    }

    bool operator!=(const ToolParameter &rhs) const {
      return !(rhs == *this);
    }

    // simple getters and setters

    const QString &getLongName() const {
      return longName;
    }

    void setLongName(const QString &longName) {
      this->longName = longName;
    }

    const QString &getName() const {
      return name;
    }

    void setName(const QString &name) {
      this->name = name;
    }

    bool isRequired() const {
      return required;
    }

    void setRequired(bool required) {
      this->required = required;
    }

    void setPositional(bool positional) {
      this->positional = positional;
    }

    const QString &getRelations() const {
      return relations;
    }

    void setRelations(const QString &relations) {
      this->relations = relations;
    }

    const QString &getInterval() const {
      return interval;
    }

    void setInterval(const QString &interval) {
      this->interval = interval;
    }

    const QString &getDescription() const {
      return description;
    }

    void setDescription(const QString &description) {
      this->description = description;
    }

    const QString &getType() const {
      return type;
    }

    void setType(const QString &type) {
      this->type = type;
    }

    const QString &getDefaultValue() const {
      return default_value;
    }

    void setDefaultValue(const QString &defaultValue) {
      default_value = defaultValue;
    }

    const QString &getValue() const {
      return value;
    }

    void setValue(const QString &value) {
      this->value = value;
    }

    const QString &isEnabled() const {
      return enabled;
    }

    void setEnabled(const QString &enabled) {
      this->enabled = enabled;
    }

    QJsonObject toJson() {
      QJsonObject jsonObject;
      jsonObject["name"] = longName;
      jsonObject["shortName"] = name;
      jsonObject["type"] = type;
      jsonObject["required"] = required;
      if (positional) {
        jsonObject["positional"] = positional;
      }
      return jsonObject;
    }

    QStringList arguments() const {
      QStringList arguments;

      if (value.isEmpty()) {
        return arguments;
      }

      if (type == "flag" || type == "bool") {
        if (value == "1") {
          if (!name.isEmpty()) {
            arguments.append("-" + name);
          } else {
            arguments.append("--" + longName);
          }
        } else {
          return arguments;
        }
      } else if (positional) {
        arguments.append(value);
      } else if (!name.isEmpty()) {
        arguments.append("-" + name);
        arguments.append(value);
      } else if (!longName.isEmpty() && !QRegularExpression(QRegularExpression::anchoredPattern("arg[0-9]")).match(longName).hasMatch()) {
        arguments.append("--" + longName);
        arguments.append(value);
      } else {
        arguments.append(value);
      }

      return arguments;
    }

    void clear() {
      longName.clear();
      name.clear();
      required = false;
      relations.clear();
      interval.clear();
      description.clear();
      type.clear();
      default_value.clear();
      value.clear();
      enabled.clear();
      positional = false;
    }

  private:
    // retrieved from xml:
    QString longName;
    QString name;
    bool required;
    QString relations;
    QString interval;
    QString description;
    QString type;
    QString default_value;
    QString value; // might be set when deserializing from xml (from tooldialog)
    QString enabled;
    bool positional;

};
