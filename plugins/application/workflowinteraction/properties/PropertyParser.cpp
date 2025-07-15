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

#include <QDebug>
#include <QtCore/QJsonObject>
#include <QtCore/QJsonArray>

#include <framework/pluginframework/pluginmanagerinterface.h>
#include <framework/pluginframework/pluginmanager.h>
#include <plugins/infrastructure/dialogs/fileopen/fileopendialoginterface.h>

#include <properties/data/ambassador.h>
#include <properties/data/properties.h>
#include <properties/data/properties/derivative/openfileproperty.h>
#include <properties/data/validator.h>
#include <properties/data/propertyfactory.h>

#ifdef WEBVIEW_SUPPORT_ENABLED
#include <properties/data/properties/controls/webviewproperty.h>
#endif

#include <plugins/infrastructure/workflows/processmanager/interaction/interactioninterface.h>
#include <QFile>
#include <QDir>

#include "PropertyParser.h"
#include "jsonserialize.h"

Property* PropertyParser::buildProperty(InteractionInterface* interaction) {
  std::string propertyName = interaction->getId().toStdString();

  ValueTypeInterfaceHint *hint;
  Property *result;

  if (interaction->getDirection() == InteractionDirection::INPUT) {
    if (interaction->getTypeIdentifier() == "cropImages") {
      auto property = new CropSelectionProperty(propertyName);
      hint = property->updateHint();
      property->setValue("imagePath", interaction->getUrl().toStdString());
      result = property;
    } else if (interaction->getTypeIdentifier() == "choose" || interaction->getTypeIdentifier() == "select") {
      std::vector<std::string> options;
      for (const auto& option : interaction->getOptions()) {
        options.push_back(option.toStdString());
      }
      auto property = new StringProperty(propertyName, (options.size() > 0) ? options[0] : "");
      hint = property->updateHint();
      hint->setValidator<ListValidator<std::string>>(ValidatorType::IN_LIST, options);
      result = property;
    } else if (interaction->getTypeIdentifier() == "periodicTable") {
      auto property = new PeriodicTableSelectionProperty(propertyName);
      hint = property->updateHint();
      result = property;
    } else if (interaction->getTypeIdentifier() == "form") {
      const QString &json_file_path = interaction->getUrl();
      const QJsonObject json = loadJson(json_file_path).object();
      auto ambassador = fromKadiTemplateJson(propertyName, json);
      hint = ambassador->updateHint();
      hint->setEntry("label.pos", "top");
      if (not json["title"].toString().isEmpty()) {
        hint->setEntry("label", json["title"].toString().toStdString()); // used for the title of the groupbox which the factory will generate
      } else {
        hint->setEntry("label", interaction->getUrl().toStdString());
      }
      result = ambassador;
    } else {
      result = PropertyFactory::createProperty(propertyName, interaction->getTypeIdentifier().toStdString(), true, interaction->getDefaultValue().toString().toStdString());
      hint = result->updateHint();
      if (interaction->isMultiline() && interaction->getTypeIdentifier() == "string") {
        hint->setWidgetIdentifier("textBox");
      }
    }
  } else if (interaction->getDirection() == InteractionDirection::OUTPUT) {
    if (interaction->getTypeIdentifier() == "webView") {
#ifdef WEBVIEW_SUPPORT_ENABLED
      auto property = new WebViewProperty(propertyName);
      hint = property->updateHint();
      hint->setEntry("direction", "output");

      // setting the value for each sub property
      std::string url = interaction->getUrl().toStdString();

      property->setValue("url", url);
      result = property;
#else
      throw std::invalid_argument("Webview widget is not supported by this version");
#endif
    } else if (interaction->getTypeIdentifier() == "string") {
      const std::string stringValue = interaction->getValue().toString().toUtf8().constData();
      auto property = new FormattedTextProperty(propertyName);
      hint = property->updateHint();
      hint->setEntry("direction", "output");
      hint->setEntry("label.pos", "none"); // the UserOutputText-Node has no label and the text is expected to be shown without additional label
      result = property;
      if (!interaction->getPrompt().isNull()) {
        property->setValue("text", interaction->getPrompt().toStdString());
      }
    } else {
      throw std::invalid_argument(
              "Unable to parse interaction of type '" + interaction->getTypeIdentifier().toStdString() + "' and direction OUTPUT");
    }
  } else {
    throw std::invalid_argument("Unable to parse interaction with unknown direction");
  }

  // use interaction description as the labelText in the property hint and enable the label by default
  const auto label = interaction->getPrompt().toStdString();
  if (!label.empty()) {
    hint->setEntry("label", label);
  }
  const auto defaultvalue = interaction->getDefaultValue();
  if (not defaultvalue.isNull()) {
    hint->setEntry("default", defaultvalue.toString().toStdString());
  }

  const auto actualvalue = interaction->getValue();
  if (not actualvalue.isNull()) {
    result->fromString(actualvalue.toString().toStdString());
  } else if (not defaultvalue.isNull()) {
    result->fromString(defaultvalue.toString().toStdString());
  }

  return result;
}

QString PropertyParser::valueFromProperty(const Property *property) {
  if (const auto bool_property = dynamic_cast<const BoolProperty*>(property)) {
    return bool_property->getValue() ? "true" : "false";
  }

  if (const auto list_property = dynamic_cast<const StringListElementProperty*>(property)) {
    return QString::fromStdString(list_property->getValue<std::string>("selected"));
  }

  // TODO das muss die ProcessEngine machen, sonst geht das nicht remote
  if (auto openfile_property = dynamic_cast<const OpenFileProperty*>(property)) {
    QString filepath = QString::fromStdString(openfile_property->toString());

    auto pluginmanager = LibFramework::PluginManager::getInstance();
    auto matchingFileDialogInterface = FileOpenDialogInterface::getCompatibleFileOpenPlugin(pluginmanager, filepath);

    if (matchingFileDialogInterface && matchingFileDialogInterface->validateAndLoadFilePath(filepath)) {
      qInfo() << "Mapping file " << filepath << " to " << matchingFileDialogInterface->getCachedFilePath();
      return matchingFileDialogInterface->getCachedFilePath();
    } else {
      return filepath;
    }
  }

  if (const auto ambassador = dynamic_cast<const Ambassador*>(property)) {
    return QString::fromStdString(model_to_json(ambassador));
  }

  return QString::fromStdString(property->toString());
}

Ambassador* PropertyParser::fromKadiTemplateJson(const std::string& propertyName, const QJsonObject& json_object) {
  auto model = new PropertiesModel(propertyName);

  if (json_object.contains("data")) {
    if (json_object["data"].isArray()) {
      // extras template
      parsePropertyArray(*model, json_object["data"].toArray());
    } else if (json_object["data"].isObject()) {
      // record template
      QJsonObject data = json_object["data"].toObject();
      if (data.contains("extras") && data["extras"].isArray()) {
        parsePropertyArray(*model, data["extras"].toArray());
      }
    }
  }
  return model;
}

bool PropertyParser::validatePropertyValues(const Ambassador* ambassador, std::vector<std::string>& invalid_values) {
  int index = 0;
  return validatePropertyValues(ambassador, invalid_values, index);
}

bool PropertyParser::validatePropertyValues(const Ambassador* ambassador, std::vector<std::string>& invalid_values, int& index) {
  bool result = true;

  for (const auto& property : ambassador->getProperties()) {
    // a simple validation for now, can be extended later
    // note: don't stop validating when invalid value found to collect all invalid values in invalid_values

    // TODO: Is there a differentiation between input and output?
    // TODO: If a required value is missing then it is missing.
    if (property->getHint()->getEntry("direction") == "output") {
      continue; // when direction is output, we don't require any input
    }

    if (auto subambassador = dynamic_cast<Ambassador*>(property.get())) {
      result &= validatePropertyValues(subambassador, invalid_values, index);
    } else {

      // TODO: This is the worst way to check if the value is empty.
      // TODO: Should be something like: property->is_valid();
      QJsonParseError parse_error;
      QJsonDocument json = QJsonDocument::fromJson(QByteArray::fromStdString(property_to_json(property.get())), &parse_error);

      if (parse_error.error == QJsonParseError::NoError) {
        const auto& value = json["value"];

        const auto is_valid = !value.isNull() && (value.isString() ? !value.toString().isEmpty() : true);
        if (!is_valid) {
          invalid_values.push_back(getString(property->getHint()->getEntry("label"), "Field " + std::to_string(index + 1)));
          result = false;
        }
      }
    }

    index++;
  }

  return result;
}

void PropertyParser::parsePropertyArray(PropertiesModel &target, const QJsonArray &json_array) {
  int index = 1;
  for (const QJsonValue &entry : json_array) {
    const QJsonObject &json_property = entry.toObject();

    const std::string name = json_property["key"].toString(QString::number(index)).toStdString();
    const std::string unit = json_property["unit"].toString().toStdString();
    QJsonValue json_value = json_property["value"];
    const std::string value = json_value.toVariant().toString().toStdString();

    std::string type = json_property["type"].toString().toStdString();

    bool isdate = false;
    if (type == "str") {
      type = "string";
    } else if (type == "date") {
      type = "string";
      isdate = true;
    }

    bool required = true;
    std::vector<std::string> options;
    if (json_property.contains("validation")) {
      QJsonObject validation = json_property["validation"].toObject();
      if (validation.contains("required")) {
        required = json_property["validation"]["required"].toBool();
      }
      if (validation.contains("options")) {
        for (QJsonValue json_option : validation["options"].toArray()) {
          std::string result;
          if (json_option.isDouble()) {
            result = QString::number(json_option.toDouble()).toStdString();
          } else if (json_option.isBool()) {
            result = json_option.toBool() ? "true" : "false";
          } else {
            result = json_option.toString().toStdString();
          }
          options.push_back(result);
        }
      }
    }

    Property *property;
    if (type == "dict" || type == "list") {
      auto sub_model = new PropertiesModel(name);
      sub_model->updateHint()->setEntry("json_type", type); // info for converting to json later
      parsePropertyArray(*sub_model, json_value.toArray());
      property = sub_model;
    } else {
      // not a nested property
      property = PropertyFactory::createProperty(name, type, required, value);
      if (isdate) {
        property->updateHint()->setWidgetIdentifier("date");
      }
    }

    if (!options.empty()) {
      ValueTypeInterfaceHint *hint = property->updateHint();
      std::stringstream validation_string;
      hint->setValidator<ListValidator<std::string>>(ValidatorType::IN_LIST, options);
    }

    property->updateHint()->setEntry("label", name);
    target.addProperty(property);
    index++;
  }
}

QJsonDocument PropertyParser::loadJson(const QString& path) {
  QString expanded_path = path;
  if (expanded_path[0] == '~') {
    expanded_path.remove(0, 1); // remove the leading ~
    expanded_path = QDir::homePath() + expanded_path;
  }
  QFile file(expanded_path);
  if (!file.open(QIODevice::ReadOnly)) {
    throw std::invalid_argument(QString("Unable to open the file \"%1\"").arg(path).toStdString());
  }
  return QJsonDocument::fromJson(file.readAll());
}

const std::string& PropertyParser::getString(const std::string& value, const std::string& fallback_value) {
  if (!value.empty()) return value;
  return fallback_value;
}
