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

#include "property.h"
#include "properties/persistence/persistentmanager.h"


bool Property::hasName(const std::string& name) const {
  return this->name == name;
}

const std::string& Property::getName() const {
  return name;
}

bool Property::makesDirty() const {
  return makesdirty;
}

void Property::store(PersistentManager* persistentmanager, const std::string& keynamespace) const {
  persistentmanager->storeProperty(this, keynamespace);
}

void Property::load(PersistentManager* persistentmanager, const std::string& keynamespace) {
  persistentmanager->loadProperty(this, keynamespace);
}

const std::string Property::getVisibleName() const {
  std::string visible_name = name;
  std::string::size_type namepos = visible_name.rfind("/");
  std::string::size_type namepos2 = visible_name.rfind(".", namepos);
  if (namepos2 != std::string::npos) {
    namepos = namepos2;
  }
  if (namepos != std::string::npos) {
    visible_name = visible_name.substr(namepos + 1);
  }
  std::replace(visible_name.begin(), visible_name.end(), '_', ' ');
  return visible_name;
}

void Property::setOwningAmbassador(Ambassador *ambassador) {
  this->ambassador = ambassador;
}
