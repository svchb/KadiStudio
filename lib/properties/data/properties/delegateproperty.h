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
#include "propertyvti.h"

/**
 * @brief      A property which maps several property names onto
 *             one property. The derived class decides what to do
 *             when one of the additional property names was requested.
 * @ingroup    properties
 */
template <typename T, typename TARGET, void (TARGET::*SetFunc)(const T& value), const T& (TARGET::*GetFunc)()>
class DelegateProperty : public PropertyVTI<ValueType<T>> {

  public:

    DelegateProperty(const std::string& name, TARGET *target, bool makesdirty = true)
        : PropertyVTI<ValueType<T>>(name, makesdirty), target(target) {}

    virtual const T& getValue() const override {
      return (target->*GetFunc)();
    }

    using ValueType<T>::fromString;
    using ValueType<T>::toString;
    using ValueType<T>::compareToString;

  private:
    virtual void setValueRaw(typename ValueTypeInterface<T>::value_type value) override {
      (target->*SetFunc)(std::move(value));
    }

    TARGET *target;
};

template <typename T, typename TARGET, void (TARGET::*SetFunc)(const T& value), const T& (TARGET::*GetFunc)()>
class ConverterDelegateProperty : public PropertyVTI<ValueTypeInterface<T>> {

  public:
    ConverterDelegateProperty(const std::string &name, TARGET *target,
                              const std::function<T(const std::string &)>& fromStringFunc,
                              const std::function<std::string(const T&)>& toStringFunc,
                              bool makesdirty = true)
        : PropertyVTI<ValueTypeInterface<T>>(name, makesdirty), target(target), fromStringFunc(fromStringFunc), toStringFunc(toStringFunc) {
    }

    const T& getValue() const override {
      return (target->*GetFunc)();
    }

    void fromString(const std::string& str) override {
      fromStringFunc(str);
    }

    std::string toString() const override {
      return toStringFunc(getValue());
    }

    bool compareToString(const std::string& /*cmp_string*/) const override {
      return true;
    }

  private:
    void setValueRaw(typename ValueTypeInterface<T>::value_type value) override {
      (target->*SetFunc)(std::move(value));
    }

    TARGET *target;
    std::function<T(const std::string &)> fromStringFunc;
    std::function<std::string(const T &)> toStringFunc;
};
