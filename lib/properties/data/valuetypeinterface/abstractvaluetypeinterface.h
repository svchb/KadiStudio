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
#include <typeinfo>

#include "../../utils/eventbus.h"
#include "../valuetypeinterfacehint.h"

#include <cpputils/globalclassmap.hpp>

/**
 * @brief      The abstract interface and base class for ValueTypeInterface
 * @ingroup    valuetypeinterface
 */
class AbstractValueTypeInterface : public GlobalClassMap {

  public:
    static constexpr std::string_view EVENT_AVTI_CHANGED{"avti_changed"};
    static constexpr std::string_view EVENT_DESTROYED{"destroyed"};

    AbstractValueTypeInterface() = default;

    virtual ~AbstractValueTypeInterface() {
      event_bus.publish(EVENT_DESTROYED, nullptr);
    }

    bool isDerivedFromBaseClass(const std::type_info& base_info) const {
      return isDerivedFromBaseClassStatic(typeid(*this), base_info);
    }

    virtual const std::type_info& getValueTypeInfo() const = 0;

    /// @brief Tries to assign the value from another value-type.
    /// @param other The other value-type.
    virtual void tryAssign(const AbstractValueTypeInterface* other);

    virtual void fromString(const std::string& str) = 0;
    virtual std::string toString() const = 0;

    virtual bool compareToString(const std::string& cmp_string) const = 0;

    virtual const ValueTypeInterfaceHint* getHint() const;
    virtual ValueTypeInterfaceHint* updateHint();

    void notify() {
      event_bus.publish(EVENT_AVTI_CHANGED, this);
    }

    template<typename F>
    [[nodiscard]] AbstractSignal::Handle onChange(F&& callback) {
      return event_bus.subscribe<AbstractValueTypeInterface*>(EVENT_AVTI_CHANGED, callback);
    }

    template<typename F>
    [[nodiscard]] AbstractSignal::Handle onDestroy(F&& callback) {
      return event_bus.subscribe<void>(EVENT_DESTROYED, callback);
    }

  protected:
    ValueTypeInterfaceHint hint;
    EventBus event_bus{};
    EventHandles event_handles{};

    template<typename U>
    void StateDerivation() {
      gClassMap[typeid(U)].insert(typeid(*this));
    }

};
