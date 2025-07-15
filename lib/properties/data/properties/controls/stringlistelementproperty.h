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
#include "../primitive/stringproperty.h"
#include "../container/vectorproperty.h"

class StringListElementProperty : public PropertiesModel {

  public:
    /**
     *  @brief Constructor of StringListElementProperty
     */
    StringListElementProperty(const std::string& name, const std::string& selected, const std::vector<std::string>& elements)
        : PropertiesModel("StringList", name) {

      addProperty(new StringProperty("selected", selected));
      addProperty(new VectorProperty<std::string>("elements", elements));
    }

    virtual ~StringListElementProperty() = default;

    std::string toString() const {
      return getProperty("selected")->toString();
    }

    void fromString(const std::string& str) {
      getProperty("selected")->fromString(str);
    }

};
