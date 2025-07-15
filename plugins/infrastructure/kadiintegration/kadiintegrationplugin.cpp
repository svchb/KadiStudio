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

#include "src/kadiintegration.h"
#include "kadiintegrationplugin.h"


void KadiIntegrationPlugin::run() {
}

void KadiIntegrationPlugin::load() {
  LibFramework::PluginManagerInterface *pluginmanager = LibFramework::PluginInterface::getPluginManager();
  kadiIntegration = new KadiIntegration(pluginmanager);
}

void KadiIntegrationPlugin::unload() {
  delete kadiIntegration;
}

LibFramework::InterfaceContainer* KadiIntegrationPlugin::createInterfaces() {
  return new LibFramework::InterfaceContainer(kadiIntegration);
}
