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
#include <set>

#include <cpputils/dllapi.hpp>

namespace LibFramework {

class PluginStateActivated;
class PluginStateLoaded;
class PluginStateUnloaded;
class PluginStateRunning;
class PluginLoader;

/**
 * @brief      Manages the information of a plugin.
 * @ingroup    framework
 */
class DLLAPI PluginInfo {

  public:

    PluginInfo(const std::string& filepath);
    ~PluginInfo();

    long getVersion() const;
    const std::string& getNamespace() const;
    const std::set<std::string>& getRequiredNamespaces() const;
    const std::string& getFilePath() const;
    const std::string& getFileName() const;
    const std::string& getClassName() const;
    const std::set<std::string>& getAuthors() const;
    const std::string& getIcon() const;
    const std::string& getHelp() const;
    const std::string& getName() const;
    const std::string& getDescription() const;

    void dump() const {
      std::cout << "Name: " << getName() << std::endl
                << "  Namespace: " << getNamespace() << std::endl
                << "  RequiredNamespaces: " << std::endl;
      for (auto const &requiredns: getRequiredNamespaces()) {
        std::cout << "    " << requiredns << std::endl;
      }
      std::cout << "  FilePath: " << getFilePath() << std::endl
                << "  FileName: " << getFileName() << std::endl
                << "  ClassName: " << getClassName() << std::endl
                << "  Icon: " << getIcon() << std::endl
                << "  Description: " << getDescription() << std::endl
                << "  Authors: " << std::endl;
      for (auto const &author: getAuthors()) {
        std::cout << "    " << author << std::endl;
      }
      std::cout << std::endl;
    }

  private:

    std::string getFileName(const std::string& path);
    void readInfos(const std::string& filename);

    long version;
    std::string namespacepath;
    std::string filepath;
    std::string filename;
    std::string classname;
    std::set<std::string> authors;
    std::string icon;
    std::string help;
    std::string name;
    std::string description;
    std::set<std::string> requirednamespaces;

};

}
