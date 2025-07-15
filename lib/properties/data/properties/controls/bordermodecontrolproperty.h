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

#include "../container/arrayproperty.h"
#include "../container/vectorproperty.h"
#include "../../propertiesmodel.h"

class BorderModeControlProperty : public PropertiesModel {

  public:
    BorderModeControlProperty(const std::vector<std::string> &borderModes)
        : PropertiesModel("BorderModeControl") {
      addProperty(new VectorProperty<std::string>("modes", borderModes));
      addProperty(new ArrayProperty<long, 6>("bordermodes", true, 0));
    }

    ~BorderModeControlProperty() = default;

};
