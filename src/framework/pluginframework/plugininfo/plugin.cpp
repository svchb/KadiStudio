/* Copyright 2022 Karlsruhe Institute of Technology
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

#include "plugin.h"
#include "plugininfo.h"
#include "pluginstatus.h"
#include "../interfacecontainer.h"

namespace LibFramework {

Plugin::Plugin(PluginInfo* const plugininfo, PluginManagerInterface* pluginmanager) {
  this->plugininfo = plugininfo;
  pluginstatus = new PluginStatus(plugininfo, pluginmanager);
}

Plugin::~Plugin() {
  delete pluginstatus;
  delete plugininfo;
}

PluginStatus* Plugin::getPluginStatus() {
  return pluginstatus;
}

const PluginStatus* Plugin::getPluginStatus() const {
  return pluginstatus;
}

const PluginInfo* Plugin::getPluginInfo() const {
  return plugininfo;
}

const InterfaceContainer* Plugin::getInterfaceContainer() const {
  return pluginstatus->getInterfaceContainer();
}

}
