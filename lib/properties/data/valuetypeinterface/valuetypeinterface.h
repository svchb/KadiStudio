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

#include "../../utils/traits.h"

#include "abstractvaluetypeinterface.h"


/**
 * @brief      Interface for all VTIs which need typed getValue/setValue functions
 * @ingroup    valuetypeinterface
 */
template <typename T>
class ValueTypeInterface : public AbstractValueTypeInterface {
  public:
    using value_type = std::remove_const_t<std::remove_reference_t<T>>;
    using reference = value_type &;
    using const_reference = const value_type &;

    static constexpr std::string_view EVENT_VALUE_CHANGED{"value_changed"};

  public:
    ValueTypeInterface() : AbstractValueTypeInterface() {
      StateDerivation<AbstractValueTypeInterface>();
    };
    ~ValueTypeInterface() = default;

    static const std::type_info& getValueTypeInfoStatic() {
      return typeid(ValueTypeInterface<value_type>);
    }

    const std::type_info& getValueTypeInfo() const override {
      return getValueTypeInfoStatic();
    }

    void setValue(value_type value) {
      setValueRaw(std::move(value));
      notify(getValue());
    }

    virtual const_reference getValue() const = 0;

    void getValue(reference value) const {
      value = getValue();
    }

    void tryAssign(const AbstractValueTypeInterface* other) override {
      // Allow assign only for values that can be copied.
      if constexpr (std::is_copy_assignable_v<T>) {
        const auto other_vti = dynamic_cast<const ValueTypeInterface<T>*>(other);
        if (other_vti != nullptr) {
          setValue(other_vti->getValue());
        } else {
          throw std::runtime_error("Value can not be assigned due to type mismatch");
        }
      } else {
        throw std::runtime_error("Value is not copy assignable");
      }
    }

    template<typename F>
    [[nodiscard]] AbstractSignal::Handle onValueChange(F&& callback) {
      return event_bus.subscribe<T>(EVENT_VALUE_CHANGED, callback);
    }

  protected:
    virtual void setValueRaw(value_type value) = 0;

    void notify(const_reference value) {
      event_bus.publish(EVENT_VALUE_CHANGED, value);
      event_bus.publish(EVENT_AVTI_CHANGED, this);
    }

};

/**
 * The ValueType<T> encapsulates a value together with a type info.
 * It does not contain a name, and this is crucial since in some cases we need Property-like constructs that do not
 * have a name (e.g. "ComplexProperty").
 * Ideally the size of the ValueType does not need more memory as the value it is encapsulating
 * (sizeof(ValueType<T>) == sizeof(T)).
 *
 * @tparam T Type of encapsulated value
 *
 * To use a custom type e.g. MyTest as T, implement the stream operators as such:
 *
 * @code
 * std::ostream& operator<<(std::ostream& os, const MyTest& t) {
 *   os << ":" << t.value;
 *   return os;
 * };
 *
 * std::istream& operator>>(std::istream& is, MyTest& t) {
 *   is >> t.value;
 *   is.clear();
 *   return is;
 * }
 * @endcode
 *
 * or you could provide your own version of toStringInternal and fromStringInternal
 *
 */
template <typename T>
class ValueType : public ValueTypeInterface<T> {
  public:
    using value_type = typename ValueTypeInterface<T>::value_type;
    using reference = typename ValueTypeInterface<T>::reference;
    using const_reference = typename ValueTypeInterface<T>::const_reference;

  public:
    ValueType()
        : ValueTypeInterface<T>() {
      AbstractValueTypeInterface::StateDerivation<ValueTypeInterface<T>>();
    }

    template<typename P = T>
    explicit ValueType(P&& value) : ValueTypeInterface<T>(), value(std::forward<P>(value)) {
      AbstractValueTypeInterface::StateDerivation<ValueTypeInterface<T>>();
    }

    ~ValueType() = default;

    using ValueTypeInterface<T>::getValue;

    /**
     * virtual for DelegateProperty and ElementTypeInterface
     */
    const_reference getValue() const override {
      return value;
    }

    void fromString(const std::string& str) override {
      fromStringInternal(str, value);
      ValueTypeInterface<T>::notify(getValue());
    }

    std::string toString() const override {
      return toStringInternal(getValue());
    }

    bool compareToString(const std::string& cmp_string) const override {
      value_type value;
      fromStringInternal(cmp_string, value);
      return (value == getValue());
    }

  protected:
    void setValueRaw(value_type v) override {
      setValueRawInternal(v);
    }

    template<typename P, typename U = std::remove_reference_t<T>>
    typename std::enable_if_t<std::is_const_v<U>, void>
    setValueRawInternal(P&& /*value*/) {
      throw std::runtime_error("ValueType::setValueRaw can not change value for type '" + std::string(typeid(T).name()) + "'.");
    }

    template<typename P, typename U = std::remove_reference_t<T>>
    typename std::enable_if_t<!std::is_const_v<U>, void>
    setValueRawInternal(P&& value) {
      this->value = std::move(value);
    }

    // T is pointing to a class deriving from AbstractValueTypeInterface
    template<typename P = T>
    typename std::enable_if<std::is_base_of<AbstractValueTypeInterface, std::remove_pointer_t<P>>::value, void>::type
    fromStringInternal(const std::string& str, reference value) const {
      if constexpr (std::is_same<std::remove_const_t<P>, std::string>::value) {
        value.fromString(str);
      } else {
        value->fromString(str);
      }
    }

    // T provides operator>>
    template<typename P = T>
    typename std::enable_if<!std::is_base_of_v<AbstractValueTypeInterface, std::remove_pointer_t<P>> && has_istream_operator<value_type>::value, void>::type
    fromStringInternal(const std::string& str, reference value) const {
      if constexpr (std::is_same<std::remove_const_t<P>, std::string>::value) {
        value = str;
      } else {
        std::stringstream stream(str);
        stream >> value;
      }
    }

    template<typename P = T>
    typename std::enable_if<!std::is_base_of<AbstractValueTypeInterface, std::remove_pointer_t<P>>::value && !has_istream_operator<value_type>::value, void>::type
    fromStringInternal(const std::string& /*str*/, reference /*value*/) const {
      //static_assert(false, "ValueType::fromStringInternal is not implemented.");
      throw std::runtime_error("ValueType::fromStringInternal is not implemented for '" + std::string(typeid(value_type).name()) + "'.");
    };

    // T is pointing to a class deriving from AbstractValueTypeInterface
    template<typename P = T>
    typename std::enable_if<std::is_base_of<AbstractValueTypeInterface, std::remove_pointer_t<P>>::value, std::string>::type
    toStringInternal(const_reference value) const {
      if constexpr (std::is_same<std::remove_pointer_t<P>, P>::value) {
        return value.toString();
      } else {
        return value->toString(); // T is pointing to a class deriving from AbstractValueTypeInterface
      }
    }

    // T provides operator<<
    template<typename P = T>
    typename std::enable_if<!std::is_base_of<AbstractValueTypeInterface, std::remove_pointer_t<P>>::value && has_ostream_operator<value_type>::value, std::string>::type
    toStringInternal(const_reference value) const {
      if constexpr (std::is_same<value_type, std::string>::value) {
        return value;
      } else {
        std::ostringstream stream;
        stream << value;
        return stream.str();
      }
    }

    template<typename P = T>
    typename std::enable_if<!std::is_base_of<AbstractValueTypeInterface, std::remove_pointer_t<P>>::value && !has_ostream_operator<value_type>::value, std::string>::type
    toStringInternal(const_reference /*value*/) const {
      // static_assert(false, "ValueType::toStringInternal<T> is not implemented.");
      throw std::runtime_error("ValueType::toStringInternal<T> is not implemented for '" + std::string(typeid(value_type).name()) + "'.");
    };

  protected:
    T value;
};

template <>
class ValueType<bool> : public ValueTypeInterface<bool> {
  public:
    ValueType() : ValueTypeInterface<bool>() {}
    explicit ValueType(bool value)
        : ValueTypeInterface<bool>(),
          value(value) {
      AbstractValueTypeInterface::StateDerivation<ValueTypeInterface<bool>>();
    }

    ~ValueType() = default;

    ValueTypeInterface<bool>::const_reference getValue() const override {
      return value;
    }

    using ValueTypeInterface<bool>::getValue;

    void fromString(const std::string& str) override {
      bool value;
      fromStringInternal(str, value);
      setValue(value); // notifies as well, in case of ElementVectorType changes also the vector component
    }

    std::string toString() const override {
      return value ? "1" : "0";
    }

    bool compareToString(const std::string& cmp_string) const override {
      bool value;
      fromStringInternal(cmp_string, value);
      return (value == getValue());
    }

  protected:
    void setValueRaw(ValueTypeInterface<bool>::value_type value) override {
      this->value = value;
    }

    void fromStringInternal(const std::string& str, bool& value) const {
      value = (str == "1") || (str == "true") || (str == "on");
    }

  protected:
    bool value;
};
