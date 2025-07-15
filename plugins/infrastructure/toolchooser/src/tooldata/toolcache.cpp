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

#include <QString>

#include "toolcache.h"


ToolDescription* ToolCache::get(const QString& toolId) {
  auto toolDescription = cache[toolId.toStdString()].get();
  return toolDescription;
}

ToolDescription* ToolCache::insert(const QString& toolId, std::unique_ptr<ToolDescription> toolDescription) {
  cache[toolId.toStdString()] = std::move(toolDescription);
  return cache[toolId.toStdString()].get();
}

void ToolCache::reset() {
  cache.clear();
}
