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

#include <type_traits>
#include "../property.h"
#include "../valuetypeinterface/valuetypeinterface.h"

/**
 * @brief   A tool to make full-featured Properties out of ValueTypeInterfaces.
 * @ingroup valuetypeinterface
 *
 * For example, PropertyVTI<VectorValueType> will behave like a VectorValueType,
 * but extends it with a name and notifies an Ambassador about changes to make it a full Property.
 *
 * @tparam TVTI VauleTypeinterface used for thie property
 */
template <typename TVTI>
class PropertyVTI : public Property, public TVTI {
  static_assert(std::is_base_of<AbstractValueTypeInterface, TVTI>::value,
                "TVTI must be a subclass of AbstractValueTypeInterface");
  public:
    template <typename ...VTIArgs>
    explicit PropertyVTI(const std::string& name, VTIArgs&& ...args)
        : Property(name, true), TVTI(args...) {
      AbstractValueTypeInterface::StateDerivationStatic<TVTI>(typeid(PropertyVTI<TVTI>));
    }

    template <typename ...VTIArgs>
    explicit PropertyVTI(const std::string& name, bool makesdirty, VTIArgs&& ...args)
        : Property(name, makesdirty), TVTI(args...) {
      AbstractValueTypeInterface::StateDerivation<TVTI>();
    }

    AbstractValueTypeInterface* getValueTypeInterface() override {
      return this;
    }

    const AbstractValueTypeInterface* getValueTypeInterface() const override {
      return this;
    }

    using TVTI::fromString;
    using TVTI::toString;
    using TVTI::updateHint;
    using TVTI::getHint;

};
