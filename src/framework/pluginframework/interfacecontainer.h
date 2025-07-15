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

#include <vector>

#include "pluginclientinterface.h"

namespace LibFramework {


/**
 * @brief      Container that encapsules all registered interfaces of a plugin.
 * @ingroup    libframework
 */
class InterfaceContainer {

  public:

    template<typename... Args>
    InterfaceContainer(Args... args) : interfaces({args...}) {
    }

    virtual ~InterfaceContainer() = default;

    /**
     * @brief Gets an interface of the plugin.
     *
     * @return Returns the requested interface or nullptr if the plugin does not
     *         provide the requested interface.
     */
    template<typename T>
    T getInterface() const {
      T iface;
      for (PluginClientInterface *clientinterface : interfaces) {
        iface = dynamic_cast<T>(clientinterface);
        if (iface) {
          return iface;
        }
      }
      return nullptr;
    }

    template<typename T>
    std::vector<T> getInterfaces() const {
      std::vector<T> ifaces;
      for (PluginClientInterface *clientinterface : interfaces) {
        T iface = dynamic_cast<T>(clientinterface);
        if (iface) {
          ifaces.push_back(iface);
        }
      }
      return ifaces;
    }

  private:

    std::vector<PluginClientInterface*> interfaces;

};

}
