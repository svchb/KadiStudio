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

#include "vectorvaluetype.h"
#include "customdelimitermixin.h"

/**
 * @brief      A ValueType container storing values of different types.
 * @ingroup    valuetypeinterface
 */
class DynamicValueType : public VectorValueType<AbstractValueTypeInterface*>, public CustomDelimiterMixin {

  public:
    DynamicValueType()
        : VectorValueType<AbstractValueTypeInterface*>() {
      AbstractValueTypeInterface::StateDerivation<VectorValueType<AbstractValueTypeInterface*>>();
    }
    explicit DynamicValueType(const std::vector<AbstractValueTypeInterface*>& value)
        : VectorValueType<AbstractValueTypeInterface*>(value) {
      AbstractValueTypeInterface::StateDerivation<VectorValueType<AbstractValueTypeInterface*>>();
    }

    DynamicValueType(const std::initializer_list<AbstractValueTypeInterface*>& value)
        : VectorValueType<AbstractValueTypeInterface*>(value) {
      AbstractValueTypeInterface::StateDerivation<VectorValueType<AbstractValueTypeInterface*>>();
    }

    ~DynamicValueType() override {
      for (auto element : value) {
        delete element;
      }
      value.clear();
    };

    /** To use fromString the vector with AbstractValueTypeInterfaces
      * must have the layout of the string. So the elements will/can not
      * be generated on the fly.
      */
    void fromString(const std::string &str) override {
      std::istringstream stream(str);
      size_t index = 0;
      std::string token;
      while (index < value.size() && std::getline(stream, token, getDelimiter(index))) {
        value[index]->fromString(token);
        index++;
      }

      notify(getValue());
    }

    std::string toString() const override {
      std::string result;
      for (size_t i = 0; i < value.size(); i++) {
        result += value[i]->toString();
        if (i + 1 < value.size()) {
          result += getDelimiter(i);
        }
      }
      return result;
    }

    bool compareToString(const std::string& /*cmp_string*/) const override {
      // std::remove_const_t<T> value;
      // fromStringInternal(cmp_string, value);
      // return (value == getValue());
      return true;
    }

    virtual void addValue(AbstractValueTypeInterface* value) {
      push_back(value);
    }

  private:

    void setValueRaw(std::vector<AbstractValueTypeInterface*> v) override {
      for (auto element : value) {
        delete element;
      }
      value.clear();
      value = std::move(v);
    }

};
