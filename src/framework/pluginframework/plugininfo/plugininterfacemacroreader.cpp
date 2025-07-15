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

#include <functional>

#include "../plugininterfacemacros.h"
#include "pluginloader.h"
#include "stringtokenizer.h"
#include "plugininterfacemacroreader.h"

namespace LibFramework {

PluginInterfaceMacroReader::PluginInterfaceMacroReader(const std::string& filename) : libraryFile(filename, std::ios::binary), filename(filename) {
  if (!libraryFile.is_open()) {
    std::cerr << "Could not read library \'" << filename << "\'." << std::endl;
    return;
  }
  std::cerr << "Loading library \'" << filename << "\'." << std::endl;
  readEntries();
  libraryFile.close();
}

PluginInterfaceMacroReader::~PluginInterfaceMacroReader() {
}

void PluginInterfaceMacroReader::readEntries() {
  while (libraryFile) {
    //search for METADATA_MAGIC
    libraryFile.ignore(std::numeric_limits<std::streamsize>::max(), METADATA_MAGIC[0]);
    if (!libraryFile) {
      return;
    }
    std::fstream::pos_type position = libraryFile.tellg();
    bool found = true;
    for (const char *it = &METADATA_MAGIC[1]; it <= &METADATA_MAGIC[sizeof(METADATA_MAGIC) - 2]; ++it) {
      const char m = *it;
      if (m != libraryFile.get()) {
        found = false;
        break;
      }
    }
    if (!found) {
      libraryFile.seekg(position + std::fstream::pos_type{1});
      continue;
    }

    //insert found entry into entries
    std::string buffer;
    std::getline(libraryFile, buffer, '\0');
    std::string::size_type delimiter = buffer.find(" ");
    metadataEntries[buffer.substr(0, delimiter)] = buffer.substr(delimiter + 1);
  }
}

const std::string PluginInterfaceMacroReader::getMetadataEntry(const std::string& key, bool required) const {
  decltype(metadataEntries)::const_iterator i = metadataEntries.find(key);
  if (i == metadataEntries.end()) {
    if (required) {
      std::cerr << metadataEntries.size() << std::endl;
      for (auto asdf : metadataEntries) {
        std::cerr << asdf.first << asdf.second << std::endl;
      }
      std::cerr << "Could not find metadata entry \'" << key << "\' in library \'" << filename << "\'." << std::endl;
    }
    return std::string();
  }
  return i->second;
}

long PluginInterfaceMacroReader::getVersion() const {
  // long version = stol(getMetadataEntry("version"));
  long version = 1; // temporary workaround
  return version;
}

std::string PluginInterfaceMacroReader::getClassName() const {
  return getMetadataEntry("classname");
}

std::set<std::string> PluginInterfaceMacroReader::getAuthors() const {
  StringTokenizer tokenizer;
  return tokenizer.tokenize(getMetadataEntry("authors"), ",");
}

std::string PluginInterfaceMacroReader::getName() const {
  std::string name = getMetadataEntry("name");
  StringTokenizer tokenizer;
  tokenizer.removeSpecialCharacters(name);
  return name;
}

std::string PluginInterfaceMacroReader::getDescription() const {
  std::string description = getMetadataEntry("description");
  StringTokenizer tokenizer;
  tokenizer.removeSpecialCharacters(description);
  return description;
}

std::string PluginInterfaceMacroReader::getIcon() const {
  std::string icon = getMetadataEntry("icon", false);
  if (!icon.empty()) {
    StringTokenizer tokenizer;
    tokenizer.removeSpecialCharacters(icon);
    return icon;
  }
  return "";
}

std::string PluginInterfaceMacroReader::getHelp() const {
  std::string help = getMetadataEntry("help", false);
  if (!help.empty()) {
    StringTokenizer tokenizer;
    tokenizer.removeSpecialCharacters(help);
    return help;
  }
  return "";
}
std::string PluginInterfaceMacroReader::getNamespace() const {
  std::string namespacepath = getMetadataEntry("namespace");
  StringTokenizer tokenizer;
  tokenizer.removeSpecialCharacters(namespacepath);
  return namespacepath;
}

std::set<std::string> PluginInterfaceMacroReader::getRequiredNamespaces() const {
  std::string namespaces = getMetadataEntry("required-namespaces", false);
  if (!namespaces.empty()) {
    StringTokenizer tokenizer;
    return tokenizer.tokenize(namespaces);
  }
  return std::set<std::string>();
}

}
