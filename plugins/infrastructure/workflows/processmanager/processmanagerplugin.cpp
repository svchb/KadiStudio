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

#include <framework/pluginframework/plugininterface.h>
#include "src/processmanager.h"

#include "processmanagerplugin.h"

void ProcessManagerPlugin::run() {
}

void ProcessManagerPlugin::load() {
  LibFramework::PluginManagerInterface *pluginmanager_interface = LibFramework::PluginInterface::getPluginManager();
  assert(pluginmanager_interface);
  interface = new ProcessManager(pluginmanager_interface);
}

void ProcessManagerPlugin::unload() {
  delete interface;
}

LibFramework::InterfaceContainer* ProcessManagerPlugin::createInterfaces() {
  return new LibFramework::InterfaceContainer(interface);
}
