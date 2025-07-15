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

#include "plugininfo.h"
#include "plugininterfacemacroreader.h"


namespace LibFramework {

PluginInfo::PluginInfo(const std::string& filepath) {
  this->filepath = filepath;
  this->filename = getFileName(filepath);
  readInfos(filepath);
}

PluginInfo::~PluginInfo() {
}

std::string PluginInfo::getFileName(const std::string& path) {
  size_t lastslash = path.find_last_of('/');

  if (lastslash == std::string::npos) {
    lastslash = 0;
  } else {
    ++lastslash;
  }

  return path.substr(lastslash);
}

long PluginInfo::getVersion() const {
  return version;
}

const std::string& PluginInfo::getNamespace() const {
  return namespacepath;
}

const std::set<std::string>& PluginInfo::getRequiredNamespaces() const {
  return requirednamespaces;
}

const std::string& PluginInfo::getFilePath() const {
  return filepath;
}

const std::string& PluginInfo::getFileName() const {
  return filename;
}

const std::string& PluginInfo::getClassName() const {
  return classname;
}

const std::set<std::string>& PluginInfo::getAuthors() const {
  return authors;
}

const std::string& PluginInfo::getIcon() const {
  return icon;
}

const std::string& PluginInfo::getHelp() const {
//  qDebug() << "PluginInfo.cpp: Helptext: " << QString::fromStdString(help);
  return help;
}

const std::string& PluginInfo::getName() const {
  return name;
}

const std::string& PluginInfo::getDescription() const {
  return description;
}

void PluginInfo::readInfos(const std::string& filename) {
  const PluginInterfaceMacroReader reader(filename);
  classname = reader.getClassName();
  authors = reader.getAuthors();
  name = reader.getName();
  description = reader.getDescription();
  icon = reader.getIcon();
  help = reader.getHelp();
  namespacepath = reader.getNamespace();
  requirednamespaces = reader.getRequiredNamespaces();
  version = reader.getVersion();
}

}
