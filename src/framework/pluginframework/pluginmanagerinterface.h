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

#include <string>
#include <vector>
#include <map>

#include "interfacecontainer.h"
#include "plugininfo/plugin.h"
#include "plugininfo/plugininfo.h"

namespace LibFramework {

typedef std::map<std::string, const InterfaceContainer*> Interfaces;

/**
 * @brief      Provides an interface for the plugins to interact with
 *             the pluginmanager.
 * @ingroup    framework
 */
class PluginManagerInterface {

  public:

    virtual ~PluginManagerInterface() = default;

    virtual std::vector<const Plugin*> getPlugins(const std::string& namespacepath = "") const = 0;
    virtual std::vector<const PluginInfo*> getPluginInfos(const std::string& namespacepath = "") const = 0;

    virtual bool load(const std::string& namespacepath) = 0;
    virtual void unload(const std::string& namespacepath) = 0;
    virtual bool isLoaded(const std::string& namespacepath) const = 0;
    virtual bool isUnloaded(const std::string& namespacepath) const = 0;
    virtual bool isRunning(const std::string& namespacepath) const = 0;
    virtual bool run(const std::string& namespacepath) = 0;
    virtual void toggle(const std::string& namespacepath) = 0;

    virtual std::vector<std::string> getRunningNamespaces() const = 0;

    virtual bool addPlugin(const std::string& filename) = 0;

    /** Returns a map with the namespaces and the InterfaceContainers found in the given namespace.
      */
    virtual Interfaces getInterfaces(const std::string& namespacepath) const = 0;

    template<typename T>
    T getInterface(const std::string& namespacepath) const {
      Interfaces interfaces = getInterfaces(namespacepath);
      const InterfaceContainer *const interfacecontainer = interfaces[namespacepath];
      if (interfacecontainer) {
        return interfacecontainer->getInterface<T>();
      }
      return nullptr;
    }

};

}
