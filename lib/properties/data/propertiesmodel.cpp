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

#include "propertiesmodel.h"


Property* PropertiesModel::addProperty(Property* const property) {
  auto it = std::find_if(properties.begin(), properties.end(), [&] (const auto &oldproperty) {
    return property->getName() == oldproperty->getName();
  });

  if (it != properties.end()) {

    // Check if the same property is added twice.
    if ((*it).get() == property) {
      return property;
    }

    properties.erase(it);
  }

  properties.emplace_back(property);

  if (property->makesDirty()) {
    setDirty(true);
  }
  property->setOwningAmbassador(this);

  try {
    auto ambassador = dynamic_cast<Ambassador*>(property);
    if (ambassador) {
      ambassador->onDomChange([this, property](const std::string& path) {
        std::string subpath;
        if (path.empty()) {
          subpath = property->getName();
        } else {
          subpath = property->getName() + '/' + path;
        }

        if (property->makesDirty()) {
          setDirty(true);
        }

        if (not isSuspended()) {
          notifyPropertyChange(property);
          event_bus.publish(EVENT_DOM_CHANGE, subpath);
          setDirty(false);
        }
      }).release();
    } else {
      property->getValueTypeInterface()->onChange([this, property]([[maybe_unused]] const auto& avti) {
        if (property->makesDirty()) {
          setDirty(true);
        }

        if (not isSuspended()) {
          notifyPropertyChange(property);
          event_bus.publish(EVENT_DOM_CHANGE, property->getName());
          setDirty(false);
        }
      }).release();
    }

  }
  catch(const std::exception& e) {
  }

  return property;
}

void PropertiesModel::removeProperty(const std::string& name) {

  auto iter = std::find_if(properties.begin(), properties.end(), [&name] (const auto &property) {
    return property->getName() == name;
  });

  if (iter != properties.end()) {
    properties.erase(iter);
  }
}
