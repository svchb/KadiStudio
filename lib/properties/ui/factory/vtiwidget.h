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

#include <stdexcept>
#include "../../data/properties/propertychangelistener.h"
#include "../../data/valuetypeinterface/valuetypeinterface.h"

class AbstractValueTypeInterface;

/**
 * @brief      A generic widget which provides a view and control to a VTI
 * @ingroup    factory
 */
class VTIWidget {

  public:
    VTIWidget(AbstractValueTypeInterface* valuetypeinterface);
    virtual ~VTIWidget() = default;

    AbstractValueTypeInterface* getValueTypeInterface() const {
      return valuetypeinterface;
    }

    /**
      * @brief Synchronizes the value with the ValueTypeInterface (VTI)
      * Every widget has to implement this method in order to initially sync
      * with its corresponding value.
      */
    virtual void synchronizeVTI() = 0;

  protected:
    template <typename T>
    void setValue(const T& value) {
      auto vti = dynamic_cast<ValueTypeInterface<T>*>(valuetypeinterface);
      if (not vti) {
        throw std::runtime_error("Could not set value (invalid VTI)");
      }
      vti->setValue(value);
    }

    void setValue(const std::string& value) {
      valuetypeinterface->fromString(value);
    }

    template <typename T>
    const T& getValue() const {
      auto vti = dynamic_cast<ValueTypeInterface<T>*>(valuetypeinterface);
      if (not vti) {
        throw std::runtime_error("Could not get value (invalid VTI)");
      }
      return vti->getValue();
    }

    template <typename T>
    void getValue(T& value) const {
      value = getValue<T>();
    }

    void getValue(std::string& value) const {
      value = valuetypeinterface->toString();
    }

    virtual bool validateValue() {
      return true;
    }

  protected:
    AbstractValueTypeInterface *valuetypeinterface;

    EventHandles signal_handler;
};
