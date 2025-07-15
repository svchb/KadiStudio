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

#pragma once

#include "plugins.h"

namespace LibFramework {

class Plugin;

/**
 * @brief      Loads, unloads or start each plugin with its dependent
 *             plugins.
 * @ingroup    framework
 */
class PluginDependencyLoader {

  public:

    PluginDependencyLoader(Plugins& plugins);
    virtual ~PluginDependencyLoader();

    bool load(Plugin* plugin);
    void unload(Plugin* plugin);
    bool isLoadable(const Plugin* plugin) const;

  private:

    void loadPluginRecursivly(Plugin* loadplugin);
    void unloadPluginRecursivly(Plugin* unloadplugin);
    bool isRequiredByLoadedPlugins(const std::string& namespacepath) const;

    Plugins &plugins;

};

}
