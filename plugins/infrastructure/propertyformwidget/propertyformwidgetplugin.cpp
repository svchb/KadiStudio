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

#include "propertyformwidgetplugin.h"
#include "src/propertyformwidget.h"

void PropertyFormWidgetPlugin::run() {
}

void PropertyFormWidgetPlugin::load() {
}

void PropertyFormWidgetPlugin::unload() {
}

LibFramework::InterfaceContainer* PropertyFormWidgetPlugin::createInterfaces() {
  LibFramework::PluginManagerInterface *pluginmanager = LibFramework::PluginInterface::getPluginManager();
  // create a new widget each time, because the ownership goes to the layout / parent widget whenever the caller decides
  // to add it to a layout
  return new LibFramework::InterfaceContainer(new PropertyFormWidget(pluginmanager));
}
