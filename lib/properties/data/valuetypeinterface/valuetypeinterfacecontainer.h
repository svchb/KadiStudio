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

#include <typeinfo>
#include "abstractvaluetypeinterface.h"

class ValueTypeInterfaceIterator;


/**
 * @brief      Interface for container VTIs which is not bound to a specific template type
 * @ingroup    valuetypeinterface
 */
class ValueTypeInterfaceContainer {
  public:
    virtual ~ValueTypeInterfaceContainer() = default;

    virtual const std::type_info& getValueTypeInfo() const = 0;
    virtual const std::type_info& getElementValueTypeInfo() = 0;

    virtual AbstractValueTypeInterface* getElementVTI(size_t key) = 0;

    using iterator = ValueTypeInterfaceIterator;

    virtual iterator begin() = 0;
    virtual iterator end() = 0;

};
