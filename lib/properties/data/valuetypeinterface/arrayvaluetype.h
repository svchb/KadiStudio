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

#include <array>
#include <regex>

#include "valuetypeinterface.h"
#include "valuetypeinterfacecontainer.h"
#include "valuetypeinterfaceiterator.h"
#include "elementtypeinterface.h"


class AbstractArrayValueType : public ValueTypeInterfaceContainer {
  public:
    AbstractArrayValueType() {
      AbstractValueTypeInterface::StateDerivationStatic<ValueTypeInterfaceContainer>(getValueTypeInfoStatic());
    }
    virtual ~AbstractArrayValueType() = default;

    static const std::type_info& getValueTypeInfoStatic() {
      return typeid(AbstractArrayValueType);
    }
};

/**
 * @brief   A ValueType storing a list of values of type T with a fixed size N.
 *
 *  To use a custom type e.g. MyTest as T, implement the stream operators as such:
 *
 *  std::ostream& operator<<(std::ostream& os, const MyTest& t) {
 *    os << ":" << t.value;
 *    return os;
 *  };
 *
 *  std::istream& operator>>(std::istream& is, MyTest& t) {
 *    is >> t.value;
 *    is.clear();
 *    return is;
 *  }
 *
 *  This works for ArrayProperty<MyTest, N> only, if you need to store pointers
 *  in the array instead, it is necessary to override the toString and
 *  fromString methods using inheritance.
 * @ingroup valuetypeinterface
 *
 * @tparam T value type of an array element
 * @tparam N size of the array
 */
template <typename T, size_t N>
class ArrayValueType : public ValueTypeInterface<std::array<T, N>>, public AbstractArrayValueType {

  public:
    using array_t = std::array<T, N>;
    using value_type = typename ValueTypeInterface<array_t>::value_type;
    using const_reference = typename ValueTypeInterface<array_t>::const_reference;

  public:
    ArrayValueType() = delete; // this constructor is dangerous because it fills value with uninitialized TVTIs

    explicit ArrayValueType(value_type value)
        : ValueTypeInterface<array_t>(), AbstractArrayValueType() {
      AbstractValueTypeInterface::StateDerivation<ValueTypeInterface<array_t>>();
      AbstractValueTypeInterface::StateDerivation<AbstractArrayValueType>();
      ArrayValueType<T, N>::setValueRaw(std::move(value));
    }

    ArrayValueType(const std::initializer_list<T>& value)
        : ArrayValueType(initializer_list2array(value)) {
    }

    explicit ArrayValueType(const T& defaultvalue)
        : ArrayValueType(initializer_default(defaultvalue)) {
    }

    virtual ~ArrayValueType() = default;

    static const std::type_info& getValueTypeInfoStatic() {
      return typeid(ArrayValueType<T, N>);
    }

    const std::type_info& getValueTypeInfo() const override {
      return getValueTypeInfoStatic();
    }

    const std::type_info& getElementValueTypeInfo() override {
      return typeid(ElementTypeInterface<array_t, T>::getValueTypeInfoStatic);
    }

    using ValueTypeInterface<array_t>::setValue;

    const_reference getValue() const override {
      return value;
    }

    using ValueTypeInterface<array_t>::getValue;

    AbstractValueTypeInterface* getElementVTI(size_t key) override {
      if (key >= value.size()) {
        throw std::invalid_argument("Index out of range: " + std::to_string(key));
      }

      auto element_type_interface = new ElementTypeInterface<array_t, T>(value, key, AbstractValueTypeInterface::updateHint());

      element_type_interface->onValueChange([this](const T&) {
        // if the element changes, call the signal_function of the whole ContainerVTI with the current value
        notify(getValue());
      }).release();

      return element_type_interface;
    }

    void fromString(const std::string& str) override {
      fromStringInternal(str, value);
      notify(getValue());
    }

    std::string toString() const override {
      std::string result = "(";
      for (size_t i = 0; i < value.size(); i++) {
        auto element_type_interface = new ElementTypeInterface<const array_t, const T>(value, i, nullptr);
        result += element_type_interface->toString();
        delete element_type_interface;
        if (i < value.size()-1) result += ",";
      }
      result += ")";
      return result;
    }

    bool compareToString(const std::string& cmp_string) const override {
      std::remove_const_t<array_t> value;
      fromStringInternal(cmp_string, value);
      if (value.size() != getValue().size()) return false;
      for (size_t i = 0; i < value.size(); i++) {
        if (value[i] != getValue()[i]) return false;
      }
      return true;
    }

    ValueTypeInterfaceContainer::iterator begin() override {
      return ValueTypeInterfaceContainer::iterator(0, this);
    }

    ValueTypeInterfaceContainer::iterator end() override {
      return ValueTypeInterfaceContainer::iterator(N, this);
    }

    T at(size_t key) {
      return value[key];
    }

  protected:
    using ValueTypeInterface<array_t>::notify;

    value_type value;

  private:
    static value_type initializer_default(const T& defaultvalue) {
      array_t value;
      value.fill(defaultvalue);
      return value;
    }

    static value_type initializer_list2array(const std::initializer_list<T>& value) {
      if (value.size() != N) {
        throw std::invalid_argument("Initializer list must have the same size as the array (" + std::to_string(N) + ")");
      }
      value_type converted;
      std::copy(value.begin(), value.end(), converted.begin());
      return converted;
    }

    void setValueRaw(value_type rawarray) override {
      value = std::move(rawarray);
    }

    void fromStringInternal(const std::string& str, typename ValueTypeInterface<array_t>::reference value) const {
      static const std::regex regex("(?:\\(|,)([^,)]*)(?:\\)|)");
      std::sregex_iterator iter(str.begin(), str.end(), regex);

      size_t columncount = std::distance(iter, std::sregex_iterator());
      if (columncount != value.size()) return; // TODO return false;

      size_t actualcolumn = 0;
      for (; iter != std::sregex_iterator(); ++iter) {
        std::string token = *(++((*iter).begin())); // first is the total match, so we want the second match
        ElementTypeInterface<array_t, T> element_type_interface(value, actualcolumn, nullptr);
        element_type_interface.fromString(token);
        actualcolumn++;
      }
    }
};
