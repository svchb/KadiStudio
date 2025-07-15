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

namespace LibFramework {

class PluginInfo;
class PluginLoader;
class InterfaceContainer;
class PluginManagerInterface;
class PluginThread;
class PluginInterface;


/**
 * @brief      Manages the status of a plugin.
 * @ingroup    libframework
 */
class PluginStatus {

  public:

    PluginStatus(PluginInfo* plugininfo, PluginManagerInterface* pluginmanager);
    ~PluginStatus();

    bool isCompatible();

    void load();
    void unload();

    bool isLoaded() const {
      return pluginstate == PluginState::loaded || pluginstate == PluginState::running;
    }

    bool isUnloaded() const {
      return pluginstate == PluginState::unloaded;
    }

    void run();

    bool isRunning() const {
      return (pluginstate == PluginState::running);
    }

    const InterfaceContainer* getInterfaceContainer();

  private:

    enum PluginState {
      unloaded,     ///< plugin loaded, plugin in unloaded state
      loaded,       ///< plugin loaded, plugin in loaded state
      running       ///< plugin loaded, plugin in running state
    };

    PluginState pluginstate;
    PluginInfo *plugininfo;
    PluginLoader *pluginloader;
    PluginManagerInterface *pluginmanager;

    PluginThread *pluginthread;
    PluginInterface *plugininterface;
    mutable InterfaceContainer *interfacecontainer;

};

}
