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

#include "valuetypeinterfacehint.h"

ValueTypeInterfaceHint* ValueTypeInterfaceHint::setDescription(const std::string& text) {
  description = text;
  return this;
}

std::string ValueTypeInterfaceHint::getDescription() const {
  return description;
}

ValueTypeInterfaceHint* ValueTypeInterfaceHint::setEntry(const std::string& key, const std::string& value) {
  entries[key] = value;
  return this;
}

ValueTypeInterfaceHint* ValueTypeInterfaceHint::setEntry(const std::string& key, const char* value) {
  entries[key] = value;
  return this;
}

ValueTypeInterfaceHint* ValueTypeInterfaceHint::setEntry(const std::string &key, float value) {
  setEntry(key, std::to_string(value));
  return this;
}

ValueTypeInterfaceHint* ValueTypeInterfaceHint::setEntry(const std::string &key, double value) {
  setEntry(key, std::to_string(value));
  return this;
}

ValueTypeInterfaceHint* ValueTypeInterfaceHint::setEntry(const std::string &key, int value) {
  setEntry(key, std::to_string(value));
  return this;
}

ValueTypeInterfaceHint* ValueTypeInterfaceHint::setEntry(const std::string& key, long value) {
  setEntry(key, std::to_string(value));
  return this;
}

ValueTypeInterfaceHint* ValueTypeInterfaceHint::setEntry(const std::string &key, bool value) {
  setEntry(key, std::to_string(value));
  return this;
}

std::string ValueTypeInterfaceHint::getEntry(const std::string &key) const {
  if (entries.find(key) == entries.end()) {
    return "";
  }
  return entries.at(key);
}

void ValueTypeInterfaceHint::getEntry(const std::string& key, std::string& value) const {
  value = getEntry(key);
}

void ValueTypeInterfaceHint::getEntry(const std::string& key, float& value) const {
  std::string rawentry = getEntry(key);
  if (!rawentry.empty()) {
    value = std::stof(getEntry(key));
  }
}

void ValueTypeInterfaceHint::getEntry(const std::string& key, double& value) const {
  std::string rawentry = getEntry(key);
  if (!rawentry.empty()) {
    value = std::stod(getEntry(key));
  }
}

void ValueTypeInterfaceHint::getEntry(const std::string& key, int& value) const {
  std::string rawentry = getEntry(key);
  if (!rawentry.empty()) {
    value = std::stoi(getEntry(key));
  }
}

void ValueTypeInterfaceHint::getEntry(const std::string& key, long& value) const {
  std::string rawentry = getEntry(key);
  if (!rawentry.empty()) {
    value = std::stol(getEntry(key));
  }
}

void ValueTypeInterfaceHint::getEntry(const std::string& key, bool& value) const {
  std::string rawentry = getEntry(key);
  if (!rawentry.empty()) {
    // "1" is true, "0" (or everything else) is false
    value = (rawentry == std::to_string(true));
  }
}

bool ValueTypeInterfaceHint::hasEntry(const std::string &key) const {
  return entries.find(key) != entries.end();
}

ValueTypeInterfaceHint* ValueTypeInterfaceHint::setWidgetIdentifier(const std::string& widgetName) {
  widgetidentifier = widgetName;
  return this;
}

std::string ValueTypeInterfaceHint::getWidgetIdentifier() const {
  return widgetidentifier;
}
