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

#include <iostream>

#include "pluginloader.h"

namespace LibFramework {

PluginLoader::PluginLoader(const std::string& filename) {
  plugin.setLoadHints(QLibrary::ExportExternalSymbolsHint);
  plugin.setFileName(QString(filename.c_str()));
}

PluginLoader::~PluginLoader() {
}

bool PluginLoader::load() {
  plugin.load();
  if (!plugin.isLoaded()) {
    std::cerr << "Could not load plugin (" << plugin.errorString().toStdString() << ")." << std::endl;
    return false;
  }
  return true;
}

bool PluginLoader::unload() {
  bool rc = plugin.unload();
  if (!rc) {
    std::cerr << "Could not unload plugin (" << plugin.errorString().toStdString() << ")." << std::endl;
  }
  return rc;
}

}
