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

#include "settingsgui.h"

#include <plugins/infrastructure/qpropertywidgetfactory/widgetfactoryinterface.h>
#include <properties/data/properties.h>
#include <properties/data/propertyfactory.h>
#include <properties/data/valuetypeinterfacehint.h>

#include <QDialogButtonBox>
#include <QVBoxLayout>
#include <QSettings>

SettingsGui::SettingsGui(QSettings* settings, WidgetFactoryInterface* wfi, QWidget* parent)
    : QDialog(parent), settings(settings) {

  auto *layout = new QVBoxLayout();
  setLayout(layout);

  model = new PropertiesModel("Plugin Settings");

  for (const QString &setting_key : settings->allKeys()) {
    std::unordered_map<int, std::string> type_mapping = {
            {QVariant::Int, "long"},
            {QVariant::Bool, "bool"},
            {QVariant::Double, "double"}
    };
    std::string type = "string";
    auto type_result = type_mapping.find(settings->value(setting_key).metaType().id());
    if (type_result != type_mapping.end()) {
      type = type_result->second;
    }
    std::string default_value = settings->value(setting_key).toString().toStdString();
    Property *property = PropertyFactory::createProperty(setting_key.toStdString(), type, false, default_value);
    ValueTypeInterfaceHint *hint = property->updateHint();
    hint->setEntry("label", setting_key.toStdString());
    model->addProperty(property);
  }
  wfi->createGui(model, this);

  auto* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel,Qt::Horizontal);

  layout->addWidget(buttonBox);

  connect(buttonBox, &QDialogButtonBox::accepted, this, &SettingsGui::saveSettings);
  connect(buttonBox, &QDialogButtonBox::rejected, this, &SettingsGui::reject);
}

void SettingsGui::saveSettings() {
  const auto &properties = model->getProperties();
  for (std::size_t i = 0; i < properties.size(); i++) {
    const Property *prop = properties[i].get();
    settings->setValue(QString::fromStdString(prop->getName()),
                       QString::fromStdString(prop->toString()));
  }
  settings->sync();
  accept();
}
