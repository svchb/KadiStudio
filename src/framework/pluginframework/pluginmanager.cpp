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

#include "pluginmanager.h"
#include "plugindependencyloader.h"
#include "plugininfo/plugin.h"
#include "plugininfo/pluginstatus.h"

template LibFramework::PluginManager* Singleton<LibFramework::PluginManager>::getInstance();

namespace LibFramework {

PluginManager::PluginManager() {
}

PluginManager::~PluginManager() {
}

bool PluginManager::addPlugin(const std::string& filename) {
  const Plugin *const existingplugin = plugins.find([&] (const Plugin *plugin) {
    const PluginInfo *const existingplugininfo = plugin->getPluginInfo();
    return filename == existingplugininfo->getFilePath();
  });

  if (existingplugin) {
    return true;
  }

  LibFramework::PluginInfo *plugininfo = new LibFramework::PluginInfo(filename);

  Plugin *plugin = new Plugin(plugininfo, this);
  PluginStatus *const pluginstatus = plugin->getPluginStatus();

  if (not pluginstatus->isCompatible()) {
    std::cerr << "Plugin " << filename << " not compatible. Implement the missing interface methods and recompile the plugin." << std::endl;
    delete plugin;
    return false;
  }

  const std::string   namespacepath = plugininfo->getNamespace();
  const Plugin *const takenplugin   = plugins.find(namespacepath);
  if (takenplugin) {
    const PluginInfo *const takenplugininfo  = takenplugin->getPluginInfo();
    const std::string       takenfilename    = takenplugininfo->getFileName();
    const std::string       filename         = plugininfo->getFileName();
    std::cerr << "PluginManager: Could not add plugin " << filename << std::endl
              << "PluginManager: Namespace already taken by plugin " << takenfilename << std::endl;
    delete plugin;
    return false;
  }

  plugins.insert(namespacepath, plugin);
  return true;
}

bool PluginManager::load(const std::string& namespacepath) {
  std::cerr << "loading plugin " << namespacepath << "..." << std::endl;
  Plugin *const plugin = plugins.find(namespacepath);
  if (plugin == nullptr) {
    return false;
  }
  PluginDependencyLoader loader(plugins);
  return loader.load(plugin);
}

void PluginManager::unload(const std::string& namespacepath) {
  Plugin *const plugin = plugins.find(namespacepath);
  if (plugin == nullptr) {
    return;
  }
  PluginDependencyLoader loader(plugins);
  loader.unload(plugin);
}

bool PluginManager::isLoaded(const std::string& namespacepath) const {
  const Plugin *const plugin = plugins.find(namespacepath);
  if (plugin == nullptr) {
    return false;
  }
  const PluginStatus *const pluginstatus = plugin->getPluginStatus();
  return pluginstatus->isLoaded();
}

bool PluginManager::isUnloaded(const std::string& namespacepath) const {
  const Plugin *const plugin = plugins.find(namespacepath);
  if (plugin == nullptr) {
    return false;
  }
  const PluginStatus *const pluginstatus = plugin->getPluginStatus();
  return pluginstatus->isUnloaded();
}

bool PluginManager::isRunning(const std::string& namespacepath) const {
  const Plugin *const plugin = plugins.find(namespacepath);
  if (plugin == nullptr) {
    return false;
  }
  const PluginStatus *const pluginstatus = plugin->getPluginStatus();
  return pluginstatus->isRunning();
}

bool PluginManager::run(const std::string& namespacepath) {
  Plugin *const plugin = plugins.find(namespacepath);
  if (plugin == nullptr) {
    return false;
  }
  PluginStatus *const pluginstatus = plugin->getPluginStatus();
  if (pluginstatus->isUnloaded()) {
    PluginDependencyLoader loader(plugins);
    if (loader.load(plugin) == false) {
      return false;
    }
  }
  pluginstatus->run();
  return true;
}

void PluginManager::toggle(const std::string& namespacepath) {
  if (namespacepath.empty()) {
    return;
  } else if (isRunning(namespacepath)) {
    unload(namespacepath);
  } else {
    run(namespacepath);
  }
}

Interfaces PluginManager::getInterfaces(const std::string& namespacepath) const {
  Interfaces interfaces;
  for (const Plugin *plugin : getPlugins(namespacepath)) {
    const InterfaceContainer *const interfacecontainer = plugin->getInterfaceContainer();
    if (interfacecontainer) {
      const PluginInfo *const plugininfo      = plugin->getPluginInfo();
      const std::string       pluginnamespace = plugininfo->getNamespace();
      interfaces[pluginnamespace] = interfacecontainer;
    }
  }
  return interfaces;
}

std::vector<std::string> PluginManager::getRunningNamespaces() const {
  std::vector<std::string> pluginnamespacevector;
  for (const Plugin *plugin : getPlugins()) {
    const PluginInfo *const plugininfo    = plugin->getPluginInfo();
    const std::string       namespacepath = plugininfo->getNamespace();
    if (isRunning(namespacepath)) {
      pluginnamespacevector.push_back(namespacepath);
    }
  }
  return pluginnamespacevector;
}

std::vector<const PluginInfo*> PluginManager::getPluginInfos(const std::string& namespacepath) const {
  std::vector<const PluginInfo*> plugininfos;
  for (const Plugin *plugin : getPlugins(namespacepath)) {
    const PluginInfo *const plugininfo = plugin->getPluginInfo();
    plugininfos.push_back(plugininfo);
  }
  return plugininfos;
}

std::vector<const Plugin*> PluginManager::getPlugins(const std::string& namespacepath) const {
  return plugins.findAll([&] (const Plugin *plugin) {
    if (namespacepath.empty()) {
      return true;
    }
    const PluginInfo *const plugininfo      = plugin->getPluginInfo();
    const std::string       pluginnamespace = plugininfo->getNamespace();
    return pluginnamespace.compare(0, namespacepath.length(), namespacepath) == 0;
  });
}

}
