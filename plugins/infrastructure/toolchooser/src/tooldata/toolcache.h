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

#include <unordered_map>
#include <memory> // std::unique_ptr
#include <string>

#include "tooldescription.h"


/**
 * @brief      Datastructure for information about a tool.
 * @ingroup    tooldialog
 */
class ToolCache {

  public:
    ToolDescription* insert(const QString& toolId, std::unique_ptr<ToolDescription> toolDescription);
    ToolDescription* get(const QString& toolId);

    void reset();

  private:
    std::unordered_map<std::string, std::unique_ptr<ToolDescription>> cache; // must be std::string on older versions, might be QString in future

};
