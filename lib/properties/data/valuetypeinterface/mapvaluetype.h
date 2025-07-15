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

#include <unordered_map>
#include <regex>

#include "valuetypeinterfacecontainer.h"
#include "valuetypeinterfaceiterator.h"
#include "valuetypeinterface.h"
#include "elementtypeinterface.h"


class AbstractMapValueType : public ValueTypeInterfaceContainer {
  public:
    AbstractMapValueType() {
      AbstractValueTypeInterface::StateDerivationStatic<ValueTypeInterfaceContainer>(getValueTypeInfoStatic());
    }
    virtual ~AbstractMapValueType() = default;

    static const std::type_info& getValueTypeInfoStatic() {
      return typeid(AbstractMapValueType);
    }
};

/**
 * @brief      A ValueType storing key-value pairs
 * @ingroup    valuetypeinterface
 */
template <typename Key, typename T>
class MapValueType : public ValueTypeInterface<std::unordered_map<Key, T>>, public AbstractMapValueType {

  public:
    using map_t = std::unordered_map<Key, T>;
    using const_reference = typename ValueTypeInterface<map_t>::const_reference;

    MapValueType()
        : ValueTypeInterface<map_t>(),
          AbstractMapValueType() {
      AbstractValueTypeInterface::StateDerivation<ValueTypeInterface<map_t>>();
      AbstractValueTypeInterface::StateDerivation<AbstractMapValueType>();
    }

    explicit MapValueType(const map_t& value)
        : MapValueType() {
      MapValueType<Key, T>::setValueRaw(value);
    }

    ~MapValueType() override = default;

    static const std::type_info& getValueTypeInfoStatic() {
      return typeid(MapValueType<Key, T>);
    }

    const std::type_info& getValueTypeInfo() const override {
      return getValueTypeInfoStatic();
    }

    const std::type_info& getElementValueTypeInfo() override {
      return typeid(ElementTypeInterface<map_t, T>::getValueTypeInfoStatic);
    }

    AbstractValueTypeInterface* getElementVTI(const Key& key) {
      if (!contains(key)) {
        throw std::invalid_argument("No entry with this key");
      }

      auto element_type_interface = new ElementTypeInterface<map_t, T>(value[key], AbstractValueTypeInterface::updateHint());

      element_type_interface->onValueChange([this](const T&) {
        // if the element changes, call the signal_function of the whole ContainerVTI with the current value
        notify(getValue());
      }).release();

      return element_type_interface;
    }

    AbstractValueTypeInterface* getElementVTI(size_t index) override {
      if (index >= value.size()) {
        throw std::invalid_argument("Index out of range: " + std::to_string(index));
      }
      const Key &key = indexToKey(getValue(), index);
      return getElementVTI(key);
    }

    void fromString(const std::string& str) override {
      fromStringInternal(str, value);
      notify(getValue());
    }

    std::string toString() const override {
      std::string result = "{";
      size_t i = 0;
      for (auto iter = getIteratorBegin(); iter != getIteratorEnd(); ++iter) {
        auto element = *iter;
        result += element.first;
        result += ":";
        const ElementTypeInterface<const map_t, const T> element_type_interface(element.second, nullptr);
        result += element_type_interface.toString();
        if (i < value.size()-1) result += ",";
        i++;
      }
      result += "}";
      return result;
    }

    bool compareToString(const std::string& cmp_string) const override {
      std::remove_const_t<map_t> cmp_value;
      fromStringInternal(cmp_string, cmp_value);
      if (cmp_value.size() != getValue().size()) return false;
      for (size_t i = 0; i < value.size(); i++) {
        Key key1 = indexToKey(cmp_value, i);
        Key key2 = indexToKey(getValue(), i);
        if (key1 != key2) return false;
        if (value.at(key1) != getValue().at(key1)) return false;
      }
      return true;
    }

    ValueTypeInterfaceContainer::iterator begin() override {
      return ValueTypeInterfaceContainer::iterator(0, this);
    }

    ValueTypeInterfaceContainer::iterator end() override {
      return ValueTypeInterfaceContainer::iterator(value.size(), this);
    }

    typename map_t::const_iterator getIteratorBegin() const {
      return value.cbegin();
    }

    typename map_t::const_iterator getIteratorEnd() const {
      return value.cend();
    }

    const map_t& getValue() const override {
      return value;
    }
    using ValueTypeInterface<map_t>::getValue;

    void remove(const Key& toerase) {
      value.erase(toerase);
      notify(value);
    }

    void clear() noexcept {
      value.clear();
      notify(value);
    }

    bool contains(Key key) const {
      return value.find(key) != value.end();
    }

    void set(Key key, const T& v) {
      value[key] = v;
      notify(value);
    }

    T at(Key key) {
      if (value.find(key) == value.end()) {
        throw std::invalid_argument("No entry with this key");
      }
      return value[key];
    }

    size_t size() const {
      return value.size();
    }

  protected:
    using ValueTypeInterface<map_t>::notify;

  private:
    void setValueRaw(map_t rawmap) override {
      value.clear();
      value = std::move(rawmap);
    }

    const Key& indexToKey(const_reference value, unsigned int index) const {
      // https://stackoverflow.com/a/7856473/3997725
      auto iter = value.cbegin();
      std::advance(iter, index);
      return iter->first;
    }

    void fromStringInternal(const std::string& str, typename ValueTypeInterface<map_t>::reference value) const {
      static const std::regex regex("(?:\\{|,)([^,}]*)(?:\\}|)");
      std::sregex_iterator iter(str.begin(), str.end(), regex);

      size_t elementcount = std::distance(iter, std::sregex_iterator());
      value.clear();
      value.reserve(elementcount);

      for (; iter != std::sregex_iterator(); ++iter) {
        std::string token = *(++((*iter).begin())); // first is the total match, so we want the second match
        auto delim = token.find(':');
        if (!delim) throw std::invalid_argument("Unable to parse token \"" + token + "\" (does not contain ':')");
        Key key;
        std::istringstream key_stream(token.substr(0, delim));
        key_stream >> key;
        value.emplace(key, T()); // make sure element for key exist
        auto element_type_interface = ElementTypeInterface<map_t, T>(value[key], nullptr);
        element_type_interface.fromString(token.substr(delim+1));
      }
    }

    map_t value;
};
