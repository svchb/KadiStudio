/* Copyright 2022 Karlsruhe Institute of Technology
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

#include "../ambassador.h"
#include "../property.h"
#include "linkedproperty.h"


Property* LinkedProperty::getProperty() const {
  if (getOwningAmbassador() == nullptr) throw std::runtime_error("Owning Ambassador of Linked property not set");
  Property *property = getOwningAmbassador()->traversePath(path);
  if (property == nullptr) {
    getOwningAmbassador()->dump();
    throw std::runtime_error("Linked property '" + path + "' not found.");
  }
  return property;
}
