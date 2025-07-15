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

#include <sstream>

#include "../../utils/traits.h"
#include "valuetypeinterface.h"


/**
 * @brief      An interface for elements of a container
 * @ingroup    valuetypeinterface
 *
 * This interface can be used like any other ValueTypeInterface
 * to set or get a value, or to call fromString() or toString(), etc.
 *
 * This is especially useful to generate VTIWidgets for elements of
 * a container derived from ValueTypeInterfaceContainer.
 */
template <typename containerT, typename T>
class ElementTypeInterface : public ValueTypeInterface<T> {
  public:
    using value_type = typename ValueTypeInterface<T>::value_type;
    using const_reference = typename ValueTypeInterface<T>::const_reference;

  public:
    explicit ElementTypeInterface(T& value, ValueTypeInterfaceHint* hint)
        : ValueTypeInterface<T>(), value(value), hint(hint) {
      AbstractValueTypeInterface::StateDerivation<ValueTypeInterface<T>>();
    }
    explicit ElementTypeInterface(containerT& container, size_t key, ValueTypeInterfaceHint* hint)
        : ElementTypeInterface(container[key], hint) {
    }

    const_reference getValue() const override {
      return value;
    }

    using ValueTypeInterface<T>::setValue;
    using ValueTypeInterface<T>::getValue;

    void fromString(const std::string& str) override {
      value_type value;
      fromStringInternal(str, value);
      setValue(value);
    }

    std::string toString() const override {
      return toStringInternal(getValue());
    }

    bool compareToString(const std::string& cmp_string) const override {
      std::remove_const_t<T> value;
      fromStringInternal(cmp_string, value);
      return (value == getValue());
    }

    virtual const ValueTypeInterfaceHint* getHint() const override {
      return hint;
    }
    virtual ValueTypeInterfaceHint* updateHint() override {
      return hint;
    }

  protected:
    void setValueRaw(value_type v) override {
      setValueRawInternal(v);
    }

    template<typename P = T>
    typename std::enable_if<std::is_same<P, const std::remove_const_t<P>>::value, void>::type
    setValueRawInternal(const T& /*v*/) {
      throw std::runtime_error("ElementTypeInterface::setValueRawInternal can not write to constant value for type '" + std::string(typeid(T).name()) + "'.");
    }

    template<typename P = T>
    typename std::enable_if<!std::is_same<P, const std::remove_const_t<P>>::value && std::is_base_of<AbstractValueTypeInterface, std::remove_pointer_t<P>>::value, void>::type
    setValueRawInternal(const T& v) {
      delete value;
      value = v;
    }

    template<typename P = T>
    typename std::enable_if<!std::is_same<P, const std::remove_const_t<P>>::value && !std::is_base_of<AbstractValueTypeInterface, std::remove_pointer_t<P>>::value, void>::type
    setValueRawInternal(const T& value) {
      this->value = value;
    }

    // T is pointing to a class deriving from AbstractValueTypeInterface
    template<typename P = T>
    typename std::enable_if<std::is_base_of<AbstractValueTypeInterface, std::remove_pointer_t<P>>::value, void>::type
    fromStringInternal(const std::string& str, std::remove_const_t<P>& value) const {
      if constexpr (std::is_same<std::remove_const_t<P>, std::string>::value) {
        value.fromString(str);
      } else {
        value->fromString(str);
      }
    }

    // T provides operator>>
    template<typename P = T>
    typename std::enable_if<!std::is_base_of<AbstractValueTypeInterface, std::remove_pointer_t<P>>::value && has_istream_operator<std::remove_const_t<P>>::value, void>::type
    fromStringInternal(const std::string& str, std::remove_const_t<P>& value) const {
      if constexpr (std::is_same<std::remove_const_t<P>, std::string>::value) {
        value = str;
      } else {
        std::stringstream stream(str);
        stream >> value;
      }
    }

    template<typename P = T>
    typename std::enable_if<!std::is_base_of<AbstractValueTypeInterface, std::remove_pointer_t<P>>::value && !has_istream_operator<std::remove_const_t<P>>::value, void>::type
    fromStringInternal(const std::string& /*str*/, std::remove_const_t<P>& /*value*/) const {
      throw std::runtime_error("ElementTypeInterface::fromStringInternal for type '" + std::string(typeid(T).name()) + "' is not implemented.");
    };

    // T is pointing to a class deriving from AbstractValueTypeInterface
    template<typename P = T>
    typename std::enable_if<std::is_base_of<AbstractValueTypeInterface, std::remove_pointer_t<P>>::value, std::string>::type
    toStringInternal(const T& value) const {
      if constexpr (std::is_same<std::remove_pointer_t<P>, P>::value) {
        return value.toString();
      } else {
        return value->toString(); // T is pointing to a class deriving from AbstractValueTypeInterface
      }
    }

    // T provides operator<<
    template<typename P = T>
    typename std::enable_if<!std::is_base_of<AbstractValueTypeInterface, std::remove_pointer_t<P>>::value && has_ostream_operator<P>::value, std::string>::type
    toStringInternal(const T& value) const {
      if constexpr (std::is_same<std::remove_const_t<P>, std::string>::value) {
        return value;
      } else {
        std::ostringstream stream;
        stream << value;
        return stream.str();
      }
    }

    template<typename P = T>
    typename std::enable_if<!std::is_base_of<AbstractValueTypeInterface, std::remove_pointer_t<P>>::value && !has_ostream_operator<P>::value, std::string>::type
    toStringInternal(const T& /*value*/) const {
      throw std::runtime_error("ElementTypeInterface::toStringInternal for type '" + std::string(typeid(T).name()) + "' is not implemented.");
      return "";
    };

    T &value;

    ValueTypeInterfaceHint *hint;
};

template <typename containerT>
class ElementTypeInterface<containerT, bool> : public ValueType<bool> {
  public:
    explicit ElementTypeInterface(containerT& container, size_t key, ValueTypeInterfaceHint* hint)
        : ValueType<bool>(container[key]), container(container), key(key), hint(hint) {
    }

    virtual const ValueTypeInterfaceHint* getHint() const override {
      return hint;
    }
    virtual ValueTypeInterfaceHint* updateHint() override {
      return hint;
    }

    const bool& getValue() const override {
      // we need to make sure that the shadow variable is syncronized with the value of the vector
      // as this can not be a reference in the case of bool
      // if this does not work in all cases we could use a mutal bool value and try to use that here
      *const_cast<bool*>(&(ValueType<bool>::value)) = container[key];
      return ValueType<bool>::getValue();
    }

  private:
    void setValueRaw(bool v) override {
      ValueType<bool>::setValueRaw(v);
      container[key] = v;
    }

  protected:

    containerT &container;
    size_t      key;

    ValueTypeInterfaceHint *hint;
};

template <typename containerT>
class ElementTypeInterface<containerT, const bool> : public ElementTypeInterface<std::remove_const_t<containerT>, bool> {
  public:
    explicit ElementTypeInterface(containerT& container, size_t key, ValueTypeInterfaceHint* hint)
        : ElementTypeInterface<std::remove_const_t<containerT>, bool>::ElementTypeInterface(*const_cast<std::remove_const_t<containerT>*>(&container), key, hint) {
    }

    void fromString(const std::string& /*str*/) override {
      throw std::runtime_error("ElementTypeInterface::fromString can not write to constant value for type '" + std::string(typeid(const bool).name()) + "'.");
    }
};
