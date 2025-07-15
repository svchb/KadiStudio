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

#include <map>
#include <vector>
#include <string>

namespace LibFramework {

class Plugin;

/**
 * @brief      A container class for plugins.
 * @ingroup    framework
 */
class Plugins {

  public:

    typedef std::map<std::string, Plugin*>::value_type value_type;

    Plugins();
    ~Plugins();

    void insert(const std::string& namespacepath, Plugin* plugin);

    Plugin* find(const std::string& namespacepath);
    const Plugin* find(const std::string& namespacepath) const;

    template<class UnaryPredicate>
    Plugin* find(UnaryPredicate pred) {
      for (const std::map<std::string, Plugin*>::value_type &pair : plugins) {
        Plugin* const plugin = pair.second;
        if (pred(plugin)) {
          return plugin;
        }
      }
      return nullptr;
    }

    template<class UnaryPredicate>
    const Plugin* find(UnaryPredicate pred) const {
      for (const std::map<std::string, Plugin*>::value_type &pair : plugins) {
        const Plugin* const plugin = pair.second;
        if (pred(plugin)) {
          return plugin;
        }
      }
      return nullptr;
    }

    template<class UnaryPredicate>
    std::vector<const Plugin*> findAll(UnaryPredicate pred) const {
      std::vector<const Plugin*> found_plugins;
      for (const std::map<std::string, Plugin*>::value_type &pair : plugins) {
        Plugin* const plugin = pair.second;
        if (pred(plugin)) {
          found_plugins.push_back(plugin);
        }
      }
      return found_plugins;
    }

  private:

    std::map<std::string, Plugin*> plugins;

};

}
