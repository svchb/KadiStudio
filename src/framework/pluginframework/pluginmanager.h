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

#include <cpputils/singleton.hpp>
#include "pluginmanagerinterface.h"
#include "plugins.h"

namespace LibFramework {

class PluginInfo;
class Plugin;


/**
 * @brief      Manages the plugins with the help of the dl library.
 * @ingroup    libframework
 */
class DLLAPI PluginManager : public PluginManagerInterface, public Singleton<PluginManager> {

  friend class Singleton<PluginManager>;

  public:

    virtual ~PluginManager();

    bool addPlugin(const std::string& filename);
    bool load(const std::string& namespacepath);
    void unload(const std::string& namespacepath);
    bool isLoaded(const std::string& namespacepath) const;
    bool isUnloaded(const std::string& namespacepath) const;
    bool isRunning(const std::string& namespacepath) const;
    bool run(const std::string& namespacepath);
    void toggle(const std::string& namespacepath);

    std::vector<const Plugin*> getPlugins(const std::string& namespacepath = "") const;
    std::vector<const PluginInfo*> getPluginInfos(const std::string& namespacepath) const;

    Interfaces getInterfaces(const std::string& namespacepath) const;

    std::vector<std::string> getRunningNamespaces() const;

  private:
    PluginManager();

    Plugins plugins;

};

}
