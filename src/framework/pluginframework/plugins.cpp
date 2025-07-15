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

#include <algorithm>

#include "plugins.h"
#include "plugininfo/plugin.h"

namespace LibFramework {

Plugins::Plugins() {
}

Plugins::~Plugins() {
  for (std::map<std::string, Plugin*>::value_type &pair : plugins) {
    Plugin *plugin = pair.second;
    delete plugin;
  }
}

void Plugins::insert(const std::string& namespacepath, Plugin* plugin) {
  plugins[namespacepath] = plugin;
}

Plugin* Plugins::find(const std::string& namespacepath) {
  std::map<std::string, Plugin*>::const_iterator it = plugins.find(namespacepath);
  if (it == plugins.cend()) {
    return nullptr;
  }
  return (*it).second;
}

const Plugin* Plugins::find(const std::string& namespacepath) const {
  std::map<std::string, Plugin*>::const_iterator it = plugins.find(namespacepath);
  if (it == plugins.cend()) {
    return nullptr;
  }
  return (*it).second;
}

}
