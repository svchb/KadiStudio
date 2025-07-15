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

#include <string>
#include "../container/vectorproperty.h"
#include "../../propertiesmodel.h"


/**
 * @brief      Property which represents a selection of elements in the periodic table.
 *             The widget factory will create a specialized widget for this property,
 *             allowing to select elements from the periodic table.
 *             The property also provides customized toString/fromString implementations.
 * @ingroup    control
 */
class PeriodicTableSelectionProperty : public PropertiesModel {

  public:
    explicit PeriodicTableSelectionProperty(const std::string& property_name);
    PeriodicTableSelectionProperty(const PeriodicTableSelectionProperty&) = delete;
    PeriodicTableSelectionProperty& operator=(const PeriodicTableSelectionProperty&) = delete;

    ~PeriodicTableSelectionProperty() override = default;

    std::string toString() const override;
    void fromString(const std::string& str) override;

private:
    VectorProperty<std::string> *selectedElements;
};
