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

#include <framework/pluginframework/pluginclientinterface.h>
#include <QObject>
class QMenu;


/**
 * @brief      Interface of the settings plugin.
 * @ingroup    settings
 */
class SettingsInterface : public LibFramework::PluginClientInterface {
 public:

  virtual void setPluginSetting(const QString& plugin_namespace,
                                const QString& key,
                                const QVariant& value) = 0;
  virtual QVariant getPluginSetting(const QString& plugin_namespace,
                                    const QString& key,
                                    const QVariant& default_value) = 0;

  virtual void addConfigMenu(const QString& plugin_namespace, QMenu* menu) = 0;
  virtual void createConfigMenu(const QString& plugin_namespace) = 0;

  ~SettingsInterface() override = default;
};
