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

#include <vector>
#include <regex>

#include "valuetypeinterface.h"
#include "valuetypeinterfacecontainer.h"
#include "valuetypeinterfaceiterator.h"
#include "elementtypeinterface.h"

class AbstractVectorValueType : public ValueTypeInterfaceContainer {
  public:
    AbstractVectorValueType() {
      AbstractValueTypeInterface::StateDerivationStatic<ValueTypeInterfaceContainer>(getValueTypeInfoStatic());
    }
    virtual ~AbstractVectorValueType() = default;

    static const std::type_info& getValueTypeInfoStatic() {
      return typeid(AbstractVectorValueType);
    }
};

/**
 * @brief      A ValueType containing multiple values of type T
 * @ingroup    valuetypeinterface
 */
template <typename T>
class VectorValueType : public ValueTypeInterface<std::vector<T>>, public AbstractVectorValueType {

  public:
    using element_t = T;
    using vector_t = std::vector<T>;
    using const_reference = typename ValueTypeInterface<vector_t>::const_reference;

    VectorValueType() : ValueTypeInterface<vector_t>(), AbstractVectorValueType() {}

    explicit VectorValueType(vector_t value)
        : VectorValueType() {
      AbstractValueTypeInterface::StateDerivation<ValueTypeInterface<vector_t>>();
      AbstractValueTypeInterface::StateDerivation<AbstractVectorValueType>();
      setValueRaw(std::move(value));
    }

    explicit VectorValueType(std::initializer_list<T> value)
        : VectorValueType(initializer_list2vector(value)) {
    }

    explicit VectorValueType(size_t size, const T& defaultvalue)
        : VectorValueType(initializer_default(size, defaultvalue)) {
    }

    ~VectorValueType() override = default;

    static const std::type_info& getValueTypeInfoStatic() {
      return typeid(VectorValueType<T>);
    }

    const std::type_info& getValueTypeInfo() const override {
      return getValueTypeInfoStatic();
    }

    const std::type_info& getElementValueTypeInfo() override {
      return ElementTypeInterface<vector_t, T>::getValueTypeInfoStatic();
    }

    AbstractValueTypeInterface* getElementVTI(size_t key) override {
      if (key >= value.size()) {
        return nullptr;
      }
      auto element_type_interface = new ElementTypeInterface<vector_t, T>(value, key, AbstractValueTypeInterface::updateHint());

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
        ElementTypeInterface<const vector_t, const T> element_type_interface(value, i, nullptr);
        result += element_type_interface.toString();
        if (i < value.size()-1) result += ",";
      }
      result += ")";
      return result;
    }

    bool compareToString(const std::string& cmp_string) const override {
      std::remove_const_t<vector_t> value;
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
      return ValueTypeInterfaceContainer::iterator(value.size(), this);
    }

    typename vector_t::const_iterator getIteratorBegin() {
      return value.cbegin();
    }

    typename vector_t::const_iterator getIteratorEnd() {
      return value.cend();
    }

    const_reference getValue() const override {
      return value;
    }
    using ValueTypeInterface<vector_t>::setValue; // TODO suspend für setValue einbauen??
    using ValueTypeInterface<vector_t>::getValue;

    void push_back(const T& val) {
      value.emplace_back(val);
      notify(value);
    }

    void push_back(T&& val) {
      value.emplace_back(val);
      notify(value);
    }

    void erase(const T& toerase) {
      value.erase(std::find_if(value.begin(), value.end(), [toerase](const T& item) {
        return item == toerase;
      }));
      notify(value);
    }

    void erase(typename vector_t::iterator *iter) {
      const T& toerase = *(*iter);
      erase(toerase);
    }

    void erase(typename vector_t::const_iterator *iter) {
      const T& toerase = *(*iter);
      erase(toerase);
    }

    void clear() noexcept {
      value.clear();
      notify(value);
    }

    void sort() {
      std::sort(value.begin(), value.end(), [](const T& left, const T& right) {
        return left < right;
      });
      notify(value);
    }

    T at(size_t key) const {
      if (key >= value.size()) {
        throw std::invalid_argument("Index out of range: " + std::to_string(key));
      }
      return value[key];
    }

    void set(size_t key, const T& v) {
      if (key >= value.size()) {
        throw std::invalid_argument("Index out of range: " + std::to_string(key));
      }
      value[key] = v;
      notify(value);
    }

    size_t size() const {
      return value.size();
    }

  protected:
    using ValueTypeInterface<vector_t>::notify;

    vector_t value;

  private:
    static vector_t initializer_default(size_t size, const T& defaultvalue) {
      return vector_t(size, defaultvalue);
    }

    static vector_t initializer_list2vector(const std::initializer_list<T>& value) {
      vector_t converted;
      std::move(value.begin(), value.end(), std::back_inserter(converted));
      return converted;
    }

    void setValueRaw(vector_t rawvector) override {
      value = std::move(rawvector);
    }

    void fromStringInternal(const std::string& str, typename ValueTypeInterface<vector_t>::reference value) const {
      static const std::regex regex("(?:\\(|,)([^,)]*)(?:\\)|)");
      std::sregex_iterator iter(str.begin(), str.end(), regex);

      size_t columncount = std::distance(iter, std::sregex_iterator());
      // const int columncount = value.size();
      // std::cout << "columncount " << columncount << std::endl;
      value.resize(columncount);

      size_t actualcolumn = 0;
      for (; iter != std::sregex_iterator(); ++iter) {
        std::string token = *(++((*iter).begin())); // first is the total match, so we want the second match
        ElementTypeInterface<vector_t, T> element_type_interface(value, actualcolumn, nullptr);
        element_type_interface.fromString(token);
        actualcolumn++;
      }
    }
};
