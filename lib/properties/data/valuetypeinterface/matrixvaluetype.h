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

#include <regex>
#include <vector>

#include "vectorvaluetype.h"
#include "valuetypeinterfaceiterator.h"
#include "valuetypeinterface.h"
#include "elementtypeinterface.h"


template <typename containerT, typename vectorT, typename T>
class ElementVectorTypeInterface : public ElementTypeInterface<containerT, vectorT>, public AbstractVectorValueType {
  public:
    explicit ElementVectorTypeInterface(containerT& value, size_t key, ValueTypeInterfaceHint* hint)
        : ElementTypeInterface<containerT, vectorT>(value, key, hint),
          AbstractVectorValueType() {
      AbstractValueTypeInterface::StateDerivation<ElementTypeInterface<containerT, vectorT>>();
      AbstractValueTypeInterface::StateDerivation<AbstractVectorValueType>();
    }

    const std::type_info& getValueTypeInfo() const override {
      return ElementTypeInterface<containerT, vectorT>::getValueTypeInfoStatic();
    }

    const std::type_info& getElementValueTypeInfo() override {
      return ElementTypeInterface<vectorT, T>::getValueTypeInfoStatic();
    }

    AbstractValueTypeInterface* getElementVTI(size_t key) override {
      if (key >= value.size()) {
        return nullptr;
      }
      auto element_type_interface = new ElementTypeInterface<vectorT, T>(value, key, updateHint());

      element_type_interface->onValueChange([this](const T&) {
        // if the element changes, call the signal_function of the whole ContainerVTI with the current value
        notify(value);
      }).release();

      return element_type_interface;
    }

    using ElementTypeInterface<containerT, vectorT>::setValue;
    using ElementTypeInterface<containerT, vectorT>::getValue;

    void fromString(const std::string& str) override {
      std::remove_const_t<vectorT> value;
      fromStringInternal(str, value);
      setValue(std::move(value));
    }

    std::string toString() const override {
      return toStringInternal(getValue());
    }

    bool compareToString(const std::string& cmp_string) const override {
      std::remove_const_t<vectorT> value;
      fromStringInternal(cmp_string, value);
      return (value == getValue());
    }

    ValueTypeInterfaceContainer::iterator begin() override {
      return ValueTypeInterfaceContainer::iterator(0, this);
    }

    ValueTypeInterfaceContainer::iterator end() override {
      return ValueTypeInterfaceContainer::iterator(value.size(), this);
    }

    using ElementTypeInterface<containerT, vectorT>::getHint;
    using ElementTypeInterface<containerT, vectorT>::updateHint;

  private:

    template<typename P = vectorT>
    typename std::enable_if<std::is_same<P, const std::remove_const_t<P>>::value, void>::type
    fromStringInternal(const std::string& /*str*/, std::remove_const_t<P>& /*value*/) const {
      throw std::runtime_error("ElementVectorTypeInterface::fromString could not write to constant value of type '" + std::string(typeid(T).name()) + "'.");
    }

    template<typename P = vectorT>
    typename std::enable_if<!std::is_same<P, const std::remove_const_t<P>>::value, void>::type
    fromStringInternal(const std::string& str, std::remove_const_t<P>& value) const {
      static const std::regex regex("(?:\\(|,)([^,)]*)(?:\\)|)");
      std::sregex_iterator iter(str.begin(), str.end(), regex);

      size_t columncount = std::distance(iter, std::sregex_iterator());
      // size_t columncount = value.size();
      value.resize(columncount);

      size_t actualcolumn = 0;
      for (; iter != std::sregex_iterator(); ++iter) {
        std::string token = *(++((*iter).begin())); // first is the total match, so we want the second match
        auto element_type_interface = ElementTypeInterface<vectorT, T>(value, actualcolumn, nullptr);
        element_type_interface.fromString(token);
        actualcolumn++;
      }
    }

    template<typename P = vectorT>
    std::string toStringInternal(const P& value) const {
      std::string result = "(";
      for (size_t i = 0; i < value.size(); i++) {
        ElementTypeInterface<const vectorT, const T> element_type_interface(value, i, nullptr);
        result += element_type_interface.toString();
        if (i < value.size()-1) result += ",";
      }
      result += ")";
      return result;
    }

    using ElementTypeInterface<containerT, vectorT>::notify;
    using ElementTypeInterface<containerT, vectorT>::value;
};


class AbstractMatrixValueType : public ValueTypeInterfaceContainer {
  public:
    AbstractMatrixValueType() {
      AbstractValueTypeInterface::StateDerivationStatic<ValueTypeInterfaceContainer>(getValueTypeInfoStatic());
    }
    virtual ~AbstractMatrixValueType() = default;

    static const std::type_info& getValueTypeInfoStatic() {
      return typeid(AbstractMatrixValueType);
    }
};

/**
 * @brief      A ValueType storing a matrix of values of type T
 * @ingroup    valuetypeinterface
 */
template <typename T>
class MatrixValueType : public ValueTypeInterface<std::vector<std::vector<T>>>, public AbstractMatrixValueType {

  public:
    using matrix_t = std::vector<std::vector<T>>;
    using vector_t = std::vector<T>;

    MatrixValueType()
        : ValueTypeInterface<matrix_t>(),
          AbstractMatrixValueType() {
      AbstractValueTypeInterface::StateDerivation<ValueTypeInterface<std::vector<std::vector<T>>>>();
      AbstractValueTypeInterface::StateDerivation<AbstractMatrixValueType>();
    }

    explicit MatrixValueType(const matrix_t& value)
        : MatrixValueType() {
      checkEqualRowSize(value);
      setValueRaw(value);
    }

    MatrixValueType(size_t rowcount, size_t columncount, const T& defaultvalue)
        : MatrixValueType() {
      setValueRaw(initializer_default(rowcount, columncount, defaultvalue));
    }

    virtual ~MatrixValueType() = default;

    static const std::type_info& getValueTypeInfoStatic() {
      return typeid(MatrixValueType<T>);
    }

    const std::type_info& getValueTypeInfo() const override {
      return getValueTypeInfoStatic();
    }

    const std::type_info& getElementValueTypeInfo() override {
      return ElementTypeInterface<vector_t, T>::getValueTypeInfoStatic();
    }

    AbstractValueTypeInterface* getElementVTI(size_t key) override {
      auto element_type_interface = new ElementVectorTypeInterface<matrix_t, vector_t, T>(value, key, AbstractValueTypeInterface::updateHint());
      element_type_interface->onValueChange([this](const vector_t&) {
        // std::cout << "MatrixValueType notify " << toString() << std::endl;
        // if the element changes, call the signal_function of the whole ContainerVTI with the current value
        notify(getValue());
      }).release();

      return element_type_interface;
    }
    AbstractValueTypeInterface* getElementVTI(size_t row, size_t column) {
      auto element_type_interface = new ElementTypeInterface<vector_t, T>(value[row], column, AbstractValueTypeInterface::updateHint());
      element_type_interface->onValueChange([this](const T&) {
        // std::cout << "MatrixValueType notify " << toString() << std::endl;
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
      std::string result = "[";
      for (unsigned int i = 0; i < value.size(); i++) {
        auto element_type_interface = ElementVectorTypeInterface<const matrix_t, const vector_t, const T>(value, i, nullptr);
        result += element_type_interface.toString();
        if (i < value.size()-1) result += ",";
      }
      result += "]";
      return result;
    }

    bool compareToString(const std::string& cmp_string) const override {
      std::remove_const_t<matrix_t> cmp_value;
      fromStringInternal(cmp_string, cmp_value);

      if (cmp_value.size() != getValue().size()) return false;
      for (size_t i = 0; i < cmp_value.size(); i++) {
        const vector_t &cmp_elementvalue = cmp_value[i];
        const vector_t &elementvalue = getValue()[i];
        if (cmp_elementvalue.size() != elementvalue.size()) return false;
        for (size_t j = 0; j < cmp_elementvalue.size(); j++) {
          if (cmp_elementvalue[j] != elementvalue[j]) return false;
        }
      }

      return true;
    }

    ValueTypeInterfaceContainer::iterator begin() override {
      return ValueTypeInterfaceContainer::iterator(0, this);
    }

    ValueTypeInterfaceContainer::iterator end() override {
      return ValueTypeInterfaceContainer::iterator(value.size(), this);
    }

    typename matrix_t::const_iterator getIteratorBegin() {
      return value.cbegin();
    }

    typename matrix_t::const_iterator getIteratorEnd() {
      return value.cend();
    }

    const matrix_t& getValue() const override {
      return value;
    }
    using ValueTypeInterface<matrix_t>::getValue;

    void addRow(const vector_t& val) {
      value.push_back(val);
      checkEqualRowSize(value);
      notify(value);
    }

    void clear() noexcept {
      value.clear();
      notify(value);
    }

    const T& at(size_t row, size_t column) const {
      if (!keyInBounds({row, column})) {
        throw std::invalid_argument("Index out of range: " + std::to_string(row) + ", " + std::to_string(column));
      }
      return value[row][column];
    }

    void set(size_t row, size_t column, const T& v) {
      if (!keyInBounds({row, column})) {
        throw std::invalid_argument("Index out of range: " + std::to_string(row) + ", " + std::to_string(column));
      }
      value[row][column] = v;
      notify(value);
    }

    size_t size() const {
      return value.size() * ((value.size()>0) ? value[0].size() : 0); // assuming all rows have the same size
    }

  private:
    void setValueRaw(matrix_t matrix) override {
      value = std::move(matrix);
    }

    bool keyInBounds(const std::pair<size_t, size_t>& key) const {
      return key.first < value.size() || (!value.empty() && key.second < value[0].size());
    }

    static matrix_t initializer_default(size_t rowcount, size_t columncount, const T& defaultvalue) {
      matrix_t result;
      result.reserve(rowcount);
      for (size_t i = 0; i < rowcount; i++) {
        std::vector<T> column(columncount, defaultvalue);
        result.push_back(column);
      }
      return result;
    }

    static void checkEqualRowSize(const matrix_t& value) {
      size_t expectedrowsize = value[0].size();
      for (size_t rowindex = 1; rowindex < value.size(); rowindex++) {
        if (value[rowindex].size() != expectedrowsize) {
          throw std::invalid_argument("Matrix rows must have the same length");
        }
      }
    }

    void fromStringInternal(const std::string& str, typename ValueTypeInterface<matrix_t>::reference value) const {
      static const std::regex regex("(?:\\[|,)(\\([^)]*\\))*(?:|\\])");
      std::sregex_iterator iter(str.begin(), str.end(), regex);

      unsigned int rowcount = std::distance(iter, std::sregex_iterator());
      // const int rowcount = value.size();
      value.resize(rowcount);

      size_t actualrow = 0;
      for (; iter != std::sregex_iterator(); ++iter) {
        std::string token = *(++((*iter).begin())); // first is the total match, so we want the second match
        ElementVectorTypeInterface<matrix_t, vector_t, T> element_type_interface(value, actualrow, nullptr);
        element_type_interface.fromString(token);
        actualrow++;
      }
    }

    using ValueTypeInterface<std::vector<std::vector<T>>>::notify;
    matrix_t value;
};
