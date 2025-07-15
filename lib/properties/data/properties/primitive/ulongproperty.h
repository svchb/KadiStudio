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

#include "../propertyvti.h"

/**
 * @note We use unsigned long instead of ulong as it is more portable
 */
class ULongProperty : public PropertyVTI<ValueType<unsigned long>> {

  public:
    ULongProperty(const std::string& name, const unsigned long value, bool makesdirty = true)
        : PropertyVTI<ValueType<unsigned long>>(name, makesdirty, value) {}

    ~ULongProperty() = default;
};
