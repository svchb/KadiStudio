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

#include <QObject>
#include <QSettings>
#include <unordered_map>

#include "settingsinterface.h"
#include <framework/pluginframework/plugininterface.h>
#include <plugins/infrastructure/qpropertywidgetfactory/widgetfactoryinterface.h>


/**
 * @brief      Manages settings for plugins.
 * @ingroup    settings
 */
class SettingsDelegate : public SettingsInterface {

 public:
  SettingsDelegate(WidgetFactoryInterface* wfi);
  ~SettingsDelegate() override;

  void setPluginSetting(const QString& plugin_namespace, const QString& key,
                        const QVariant& value) override;
  QVariant getPluginSetting(const QString& plugin_namespace,
                            const QString& key,
                            const QVariant& default_value) override;

  void addConfigMenu(const QString& plugin_namespace, QMenu* menu) override;
  void createConfigMenu(const QString& plugin_namespace) override;

 private:

  WidgetFactoryInterface *wfi;
  QSettings* getPluginSettings(const QString& plugin_namespace);

  std::unordered_map<QString, QSettings*> plugin_settings;
};
