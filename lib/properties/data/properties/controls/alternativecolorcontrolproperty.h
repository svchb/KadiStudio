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

#include "../../propertiesmodel.h"
#include "stringlistelementproperty.h"

class AlternativeColorControlProperty : public PropertiesModel {

  public:
    AlternativeColorControlProperty(const std::vector<std::string> &extensions)
        : PropertiesModel("AlternativeColorControl") {

      addProperty(new StringListElementProperty("colorMode", "default", { "default", "normal", "file" }));

      std::string selectedextension = extensions.empty() ? "" : extensions.front();
      addProperty(new StringListElementProperty("extensions", selectedextension, extensions));
    }

};
