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

#include "propertyformwidgetinterface.h"

/**
 * @brief      A plugin providing the PropertyFormWidget, a widget
 *             allowing a user to edit multiple property values at
 *             once.
 * @ingroup    propertyformwidget
 */
class PropertyFormWidgetPlugin : public LibFramework::PluginInterface {

  public:
    void run() override;
    void load() override;
    void unload() override;

    LibFramework::InterfaceContainer* createInterfaces() override;

  private:
    PropertyFormWidgetInterface *interface;
};

PLUGIN_INSTANCE(PropertyFormWidgetPlugin)
PLUGIN_AUTHORS(
  Markus Maier,
  Hensgen O
)
PLUGIN_NAME(PropertyFormWidget)
PLUGIN_DESCRIPTION(A plugin providing the PropertyFormWidget - a widget allowing a user to edit multiple property values at once)
PLUGIN_NAMESPACE(/plugins/infrastructure/propertyformwidget)
PLUGIN_REQUIRED_NAMESPACES(/plugins/infrastructure/qpropertywidgetfactory)
