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
#include <algorithm>
#include <string>
#include <set>

#include "plugininfo/plugin.h"
#include "plugininfo/pluginstatus.h"
#include "plugininfo/plugininfo.h"

#include "plugindependencyloader.h"

namespace LibFramework {

PluginDependencyLoader::PluginDependencyLoader(Plugins& plugins) : plugins(plugins) {
}

PluginDependencyLoader::~PluginDependencyLoader() {
}

bool PluginDependencyLoader::load(Plugin* const plugin) {
  if (!isLoadable(plugin)) {
    return false;
  }
  loadPluginRecursivly(plugin);
  return true;
}

void PluginDependencyLoader::loadPluginRecursivly(Plugin* const loadplugin) {
  const PluginInfo *const loadplugininfo = loadplugin->getPluginInfo();
  const std::set<std::string> requiredplugins = loadplugininfo->getRequiredNamespaces();

  for (const std::string &requiredplugin : requiredplugins) {
    Plugin *const plugin = plugins.find(requiredplugin);
    loadPluginRecursivly(plugin);
  }

  PluginStatus *const loadpluginstatus = loadplugin->getPluginStatus();
  loadpluginstatus->load();
}

void PluginDependencyLoader::unload(Plugin* const plugin) {
  const PluginStatus *const pluginstatus = plugin->getPluginStatus();
  if (pluginstatus->isLoaded()) {
    unloadPluginRecursivly(plugin);
  }
}

void PluginDependencyLoader::unloadPluginRecursivly(Plugin* const unloadplugin) {
  PluginStatus *const unloadpluginstatus = unloadplugin->getPluginStatus();
  unloadpluginstatus->unload();

  const PluginInfo *const unloadplugininfo = unloadplugin->getPluginInfo();
  const std::set<std::string> unloadpluginnamespaces = unloadplugininfo->getRequiredNamespaces();

  for (const std::string &unloadpluginnamespace : unloadpluginnamespaces) {
    if (!isRequiredByLoadedPlugins(unloadpluginnamespace)) {
      Plugin *const plugin = plugins.find(unloadpluginnamespace);
      unloadPluginRecursivly(plugin);
    }
  }
}

bool PluginDependencyLoader::isRequiredByLoadedPlugins(const std::string& namespacepath) const {
  const Plugin *const loaded_plugin = plugins.find([&] (const Plugin *plugin) {
    const PluginStatus *const pluginstatus = plugin->getPluginStatus();
    const PluginInfo *const plugininfo = plugin->getPluginInfo();
    const std::string pluginnamespacepath = plugininfo->getNamespace();
    if (pluginnamespacepath != namespacepath && pluginstatus->isLoaded()) {
      const std::set<std::string> requiredpluginnamespaces = plugininfo->getRequiredNamespaces();
      if (std::find(requiredpluginnamespaces.cbegin(), requiredpluginnamespaces.cend(), namespacepath) != requiredpluginnamespaces.end()) {
        return true;
      }
    }
    return false;
  });
  return (loaded_plugin != nullptr);
}

bool PluginDependencyLoader::isLoadable(const Plugin* const plugin) const {
  const PluginInfo *const plugininfo = plugin->getPluginInfo();
  const std::set<std::string> requiredpluginnamespaces = plugininfo->getRequiredNamespaces();

  for (const std::string &requiredpluginnamespace : requiredpluginnamespaces) {
    if (plugins.find(requiredpluginnamespace) == nullptr) {
      std::cerr << "PluginDependencyLoader: Namespace of required plugin not found: " << requiredpluginnamespace << std::endl
                << "PluginDependencyLoader: Was the shared library of the required plugin build?" << std::endl;
      return false;
    }
  }

  for (const std::string &requiredpluginnamespace : requiredpluginnamespaces) {
    const Plugin *const requiredplugin = plugins.find(requiredpluginnamespace);
    if (!isLoadable(requiredplugin)) {
      return false;
    }
  }

  return true;
}

}
