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

#include <framework/pluginframework/plugininterface.h>
#include <plugins/infrastructure/qpropertywidgetfactory/widgetfactoryinterface.h>

class SettingsDelegate;

/**
 * @brief      A plugin to manage settings for other plugins.
 * @ingroup    settings
 */
class SettingsPlugin : public LibFramework::PluginInterface {

  public:
    void run() override;
    void load() override;
    void unload() override;

    LibFramework::InterfaceContainer* createInterfaces() override;

  private:
    SettingsDelegate *delegate;
};

PLUGIN_INSTANCE(SettingsPlugin)
PLUGIN_AUTHORS(
Michael Kirmizakis
)
PLUGIN_NAME(Settings Plugin)
PLUGIN_DESCRIPTION(A plugin to manage settings for other plugins)
PLUGIN_ICON(:/studio/plugins/infrastructure/settings/icons/fa-sliders-h.svg)
PLUGIN_NAMESPACE(/plugins/infrastructure/settingsplugin)
PLUGIN_REQUIRED_NAMESPACES(
  /plugins/infrastructure/qpropertywidgetfactory
)
