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

#include "periodictableselectionproperty.h"
#include "../container/vectorproperty.h"

PeriodicTableSelectionProperty::PeriodicTableSelectionProperty(const std::string& property_name)
    : PropertiesModel("PeriodicTableSelection", property_name) {

  selectedElements = addProperty(new VectorProperty<std::string>("selectedElements"));
}

std::string PeriodicTableSelectionProperty::toString() const {
  std::string result = selectedElements->toString();
  if (*result.begin() == '(' && *(result.end() - 1) == ')') {
    return result.substr(1, result.size() - 2);
  }
  return result;
}

void PeriodicTableSelectionProperty::fromString(const std::string &str) {
  std::string stringValue = str;
  if (*str.begin() != '(' && *(str.end() - 1) != ')') {
    stringValue = "(" + stringValue + ")";
  }
  selectedElements->fromString(stringValue);
}
