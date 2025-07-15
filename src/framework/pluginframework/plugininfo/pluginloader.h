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

#include <iostream>
#include <string>
#include <functional>

#include <QtCore/QLibrary>

namespace LibFramework {

/**
 * @brief      Loads and unloads shared objects / plugins.
 * @ingroup    libframework
 */
class PluginLoader {

  public:

    PluginLoader(const std::string& filename);
    ~PluginLoader();

    bool load();
    bool unload();
    bool isLoaded() const {
      return plugin.isLoaded();
    }

    std::string getFilename() const {
      return plugin.fileName().toStdString();
    }

    template<typename Signature>
    std::function<Signature> getFunctionPointer(const std::string& functionname, bool required = true) {
      std::function<Signature> function;
      QFunctionPointer functionaddress = plugin.resolve(functionname.c_str());
      if (functionaddress) {
        function = reinterpret_cast<Signature*>(functionaddress);
      } else if (required) {
        std::cerr << "Could not load symbole " << functionname << " from plugin " << plugin.fileName().toStdString() << " (" << plugin.errorString().toStdString() << ")." << std::endl;
        return nullptr;
      }
      return function;
    }

  private:

    QLibrary plugin;

};

}
