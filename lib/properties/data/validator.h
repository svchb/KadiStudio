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
#include <sstream>
#include <type_traits>
#include <algorithm>

#include <cpputils/string.hpp>


enum class ValidatorType {
  IN_RANGE,
  NOT_IN_RANGE,
  IN_LIST,
  NOT_IN_LIST
};

/**
 * @brief      A validator for VTIs which can be stored in the hint
 * @ingroup    data
 */
class ValueValidator {
  public:
    explicit ValueValidator(ValidatorType type) : type(type) {
    }
    virtual ~ValueValidator() = default;
    virtual bool validateValue(const std::string& value) const = 0;
    ValidatorType getValidatorType() const {
      return type;
    }

  protected:
    ValidatorType type;
};

/**
 * @brief      A list validator for VTIs which can be stored in the hint
 * @ingroup    data
 */
template <typename T>
class ListValidator : public ValueValidator {
  public:

    template <typename... Args>
    explicit ListValidator(ValidatorType type, Args... args) : ValueValidator(type), list({args...}) {
      if (type != ValidatorType::IN_LIST && type != ValidatorType::NOT_IN_LIST) {
        throw std::invalid_argument("ListValidator only supports validator types IN_LIST and NOT_IN_LIST");
      }
    }

    void setList(const std::vector<T>& list) {
      this->list = list;
    }

    const std::vector<T>& getList() const {
      return list;
    }

    bool validateValue(const std::string& value) const override {
      return validateValueInternal(value);
    }

  private:
    template <typename P = T, typename std::enable_if<!std::is_same<P, std::string>::value, bool>::type = true>
    bool validateValueInternal(const std::string& value) const {
      T t;
      std::istringstream token_stream(value);
      if (token_stream >> t) {
        bool in_list = std::find(list.begin(), list.end(), t) != list.end();
        return valid(in_list);
      }
      return false;
    }

    template <typename P = T, typename std::enable_if<std::is_same<P, std::string>::value, bool>::type = true>
    bool validateValueInternal(const std::string& value) const {
      bool in_list = std::find(list.begin(), list.end(), cpputils::trim<std::string>(value, " \t").c_str()) != list.end();
      return valid(in_list);
    }

    bool valid(bool in_list) const {
      return (type == ValidatorType::IN_LIST && in_list) || (type == ValidatorType::NOT_IN_LIST && !in_list);
    }

    std::vector<T> list;

};
