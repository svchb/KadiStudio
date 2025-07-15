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

#include <functional>
#include "../property.h"
#include "../valuetypeinterface/valuetypeinterface.h"

class FunctionProperty : public Property, public ValueTypeInterface<const std::function<void()>> {

  public:

    FunctionProperty(const std::string& name, const std::function<void()>& signal_function = []() {})
        : Property(name, false), ValueTypeInterface<const std::function<void()>>() {
      setValueRaw(signal_function);
    }

    ~FunctionProperty() = default;

    AbstractValueTypeInterface* getValueTypeInterface() override {
      return this;
    }

    const AbstractValueTypeInterface* getValueTypeInterface() const override {
      return this;
    }

    void fromString(const std::string&) override {
    }

    std::string toString() const override {
      return "";
    }

    bool compareToString(const std::string& /*cmp_string*/) const override {
      return true;
    }

    void callUpdateFunction() {
      signal_function();
    }

    const_reference getValue() const override {
      return signal_function;
    }

  private:
    void setValueRaw(ValueTypeInterface<const std::function<void()>>::value_type signal_function) override {
      this->signal_function = std::move(signal_function);
    }

    std::function<void()> signal_function;

};
