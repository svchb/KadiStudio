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

#include <QMenu>

#include <framework/pluginframework/plugininterface.h>

#include "testplugincommandlineparser.h"

class PropertiesTestGui;
class DockWindow;

class TestPlugin : public LibFramework::PluginInterface, public TestPluginCommandlineParser {

  public:
    void run() override;
    void load() override;
    void unload() override;

    LibFramework::InterfaceContainer* createInterfaces() override;

  private:
    PropertiesTestGui *sTestGui;
    QMenu *menu;
    DockWindow *dockwindow;

};

PLUGIN_INSTANCE(TestPlugin)
PLUGIN_AUTHORS(
Markus Maier,
Jens Frederich
)
PLUGIN_NAME(Test Plugin)
PLUGIN_DESCRIPTION(A little test plugin to understand the plugin structure)
PLUGIN_ICON(:/studio/framework/application/pixmaps/noicon.png)
PLUGIN_HELP(:/studio/plugins/application/testplugin/help/help.txt)
PLUGIN_NAMESPACE(/plugins/application/testplugin)
PLUGIN_REQUIRED_NAMESPACES(
  /plugins/infrastructure/qpropertywidgetfactory
  /plugins/infrastructure/settingsplugin
)
