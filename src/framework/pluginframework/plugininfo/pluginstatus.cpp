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

#include <iostream>
#include <stdexcept>

#include "../pluginmanagerinterface.h"
#include "../interfacecontainer.h"
#include "../plugininterface.h"

#include "pluginthread.h"
#include "plugininfo.h"
#include "pluginloader.h"
#include "pluginstatus.h"


namespace LibFramework {

PluginStatus::PluginStatus(PluginInfo* const plugininfo, PluginManagerInterface* pluginmanager) {
  this->plugininfo = plugininfo;
  this->pluginmanager = pluginmanager;
  const std::string filepath = plugininfo->getFilePath();
  pluginloader = new PluginLoader(filepath);
  pluginstate = PluginState::unloaded;

  plugininterface = nullptr;
  pluginthread = nullptr;
  interfacecontainer = nullptr;
}

PluginStatus::~PluginStatus() {
  if (isLoaded()) {
    unload();
  }
  if (plugininterface) {
    delete interfacecontainer;
    delete plugininterface;
    pluginloader->unload();
  }

  delete pluginloader;
}

bool PluginStatus::isCompatible() {
  long pluginversion = plugininfo->getVersion();
  long requiredversion = PLUGIN_VERSION;
  return pluginversion == requiredversion;
}

void PluginStatus::load() {
  if (!isLoaded()) {
    if (!plugininterface) {
      if (not pluginloader->load()) return;

      std::function<PluginInterface* ()> plugininstancefunction = pluginloader->getFunctionPointer<PluginInterface* ()>("createInstance");
      if (plugininstancefunction) plugininterface = plugininstancefunction();
      if (plugininstancefunction == nullptr || plugininterface == nullptr) {
        std::cerr << "PluginInterfaceMacroReader: Failed to create plugin: instance function not found." << std::endl
                  << "PluginInterfaceMacroReader: Can't get plugin interface of plugin " << plugininfo->getFilePath() << std::endl;
        return;
      }
      plugininterface->setPluginManager(pluginmanager);
    }
    plugininterface->load();
    pluginstate = PluginState::loaded;
  }
}

void PluginStatus::unload() {
  if (isLoaded()) {
    plugininterface->unload();
    delete pluginthread;
    pluginthread = nullptr;
    pluginstate = PluginState::unloaded;
  }
}

void PluginStatus::run() {
  if (!isLoaded()) {
    pluginmanager->load(plugininfo->getNamespace());
  }

  if (!pluginthread) {
    pluginthread = new PluginThread(plugininterface);
  }
  pluginthread->start();

  pluginstate = PluginState::running;
}

const InterfaceContainer* PluginStatus::getInterfaceContainer() {
  if (!isLoaded()) {
    // load plugins and its dependencies
    pluginmanager->load(plugininfo->getNamespace());
  }
  if (!plugininterface) {
    throw std::runtime_error("pluginstatus: plugin instance was null!");
  }

  delete interfacecontainer;
  interfacecontainer = plugininterface->createInterfaces();
  return interfacecontainer;
}

}
