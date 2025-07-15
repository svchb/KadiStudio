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

#include <string>
#include <unordered_map>
#include <memory>
#include "validator.h"


/**
 * @ingroup    data
 * @brief      A class holding additional information which help to display and arrange widgets for properties.
 */
class ValueTypeInterfaceHint {
  public:
    ValueTypeInterfaceHint* setDescription(const std::string& text);
    std::string getDescription() const;

    ValueTypeInterfaceHint* setEntry(const std::string& key, const std::string& value);
    ValueTypeInterfaceHint* setEntry(const std::string& key, const char* value); // for string literals
    ValueTypeInterfaceHint* setEntry(const std::string& key, float value);
    ValueTypeInterfaceHint* setEntry(const std::string& key, double value);
    ValueTypeInterfaceHint* setEntry(const std::string& key, int value);
    ValueTypeInterfaceHint* setEntry(const std::string& key, long value);
    ValueTypeInterfaceHint* setEntry(const std::string& key, bool value);
    std::string getEntry(const std::string& key) const;
    void getEntry(const std::string& key, std::string& value) const;
    void getEntry(const std::string& key, float& value) const;
    void getEntry(const std::string& key, double& value) const;
    void getEntry(const std::string& key, int& value) const;
    void getEntry(const std::string& key, long& value) const;
    void getEntry(const std::string& key, bool& value) const;

    template<typename T>
    T getEntry(const std::string& key) const {
      T value{};
      getEntry(key, value);
      return value;
    }

    template<typename T>
    T getEntryOrDefault(const std::string& key, T default_entry) const {
      return hasEntry(key) ? getEntry<T>(key) : default_entry;
    }

    bool hasEntry(const std::string& key) const;

    ValueTypeInterfaceHint* setWidgetIdentifier(const std::string& widgetName);
    /**
     * Return the identifier of a widget that should be used to represent this property.
     * If the name is empty (or there is no ValueTypeInterfaceHint present), this means that the
     * widget can be determined by the type of the property or the type of the value.
     *
     * @return Identifier of a widget which must be registered in the widget factory, or empty string
     */
    std::string getWidgetIdentifier() const;

    template <typename T, typename... Args>
    void setValidator(Args&&... args) {
      validator = std::make_unique<T>(std::forward<Args>(args)...);
      // TODO syncronizeVTI/synchronizeHint...
    }

    template <typename T>
    const T *getValidator() const {
      return dynamic_cast<const T*>(validator.get());
    }

  private:
    std::string description;
    std::string widgetidentifier;
    std::unique_ptr<ValueValidator> validator;

    std::unordered_map<std::string, std::string> entries;
};
