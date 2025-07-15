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

#include <fstream>
#include <map>
#include <string>
#include <set>

namespace LibFramework {

class PluginInterface;
class PluginMetadataLoader;


/**
 * @brief      Reads information from the macros of the plugin interface.
 * @ingroup    framework
 */
class PluginInterfaceMacroReader {

  public:

    PluginInterfaceMacroReader(const std::string& filename);
    ~PluginInterfaceMacroReader();

    long getVersion() const;
    std::string getClassName() const;
    std::set<std::string> getAuthors() const;
    std::string getName() const;
    std::string getDescription() const;
    std::string getIcon() const;
    std::string getHelp() const;
    std::string getNamespace() const;
    std::set<std::string> getRequiredNamespaces() const;

  private:
    const std::string getMetadataEntry(const std::string& key, bool required = true) const;

    void readEntries();
    std::ifstream libraryFile;
    std::map<std::string, std::string> metadataEntries;
    const std::string filename;

};

}
