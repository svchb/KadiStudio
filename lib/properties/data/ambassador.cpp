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

#include <iostream>

#include "properties/linkedproperty.h"
#include "properties/propertychangelistener.h"

#include "ambassador.h"

// #define DEBUG_AMBASSADOR

Ambassador::Ambassador(const std::string& modelname, const std::string& name)
    : Property(name, true),
      modelname(modelname),
      is_dirty(false) {
  registerPropertyChangeListener(this);
}

Ambassador::~Ambassador() {
  removePropertyChangeListener(this);
}

void Ambassador::store(PersistentManager* persistentmanager, const std::string& keynamespace) const {
  std::string ambassadorkeynamespace = keynamespace + getName() + ".";
  for (const auto &property : getProperties()) {
    property->store(persistentmanager, ambassadorkeynamespace);
  }
}

void Ambassador::load(PersistentManager* persistentmanager, const std::string& keynamespace) {
  std::string ambassadorkeynamespace = keynamespace + getName() + ".";
  for (const auto &property : getProperties()) {
    std::cout << "loading property " << ambassadorkeynamespace << property->getName() << std::endl;
    property->load(persistentmanager, ambassadorkeynamespace);
    if (property->makesDirty()) {
      setDirty(true);
    }
    notifyPropertyChange(property.get());
    std::cout << "        with value " << property->toString() << std::endl;
  }
  if (isDirty()) clean();
}

bool Ambassador::hasProperty(const std::string& path) const {
  try {
    return (traversePath(cpputils::split(path, "/")) != nullptr);
  } catch (...) {
    return false;
  }
}

void Ambassador::registerPropertyChangeListener(PropertyChangeListener* const listener) {
  if (!listener) {
    throw std::runtime_error("nullptr registered as property listener");
  }
#ifdef DEBUG_AMBASSADOR
  std::cerr << " (DD) register listener " << listener->getListenerName() << " on ambassador " << getName() << std::endl;
#endif
  listeners.insert(listener);
}

void Ambassador::removePropertyChangeListener(PropertyChangeListener* const listener) {
#ifdef DEBUG_AMBASSADOR
  std::cerr << " (DD) remove listener " << listener->getListenerName() << " on ambassador " << getName() << std::endl;

  if (!listeners.empty() && (listeners.find(listener) != listeners.end())) {
    std::cerr << "There is no such listener to remove!" << std::endl;
    exit(1);
  }
#endif
  listeners.erase(listener);
}

void Ambassador::notifyPropertyChange(const Property* property) {
#ifdef DEBUG_AMBASSADOR
  std::cerr << " (DD) '" << getName() << "' notifies about change of property '" << property->getName() << "' with type '" << property->getValueTypeInfo().name() << std::endl;
#endif

  for (PropertyChangeListener *listener : listeners) {
#ifdef DEBUG_AMBASSADOR
    std::cerr << " (DD) sending to listener " << listener->getListenerName() << std::endl;
#endif
    listener->receivePropertyChange(property);
  }
}

Property* Ambassador::traversePath(const std::vector<std::string>& path) const {
  const Ambassador *current_ambassador{this};
  const Property *current_property{nullptr};

  for (auto iter = path.begin(); iter != path.end(); iter++) {
    const auto& segment = *iter;

    // do nothing
    if (segment == ".") {
    }
    // Traverse up.
    else if (segment == "..") {

      if (current_ambassador->getOwningAmbassador() == nullptr) {
        throw std::runtime_error("Ilegal path");
      }

      current_ambassador = current_ambassador->getOwningAmbassador();
    }
    // Traverse down.
    else {
      auto next_property = current_ambassador->getProp(segment);
      auto next_property_vti = next_property->getValueTypeInterface();

      if (dynamic_cast<const ValueTypeInterface<Ambassador>*>(next_property_vti)) {
        current_ambassador = dynamic_cast<Ambassador*>(next_property_vti);
      } else if (std::next(iter) != path.end()) {
        // A "primitive" property is accessed but the path has not been fully traversed.
        throw std::runtime_error("Is not an Ambassador");
      }

      current_property = next_property;
    }
  }

  return const_cast<Property*>(current_property);
}

Ambassador* Ambassador::getRootAmbassador() const {
  const Ambassador *current_ambassador = this;

  while (current_ambassador->getOwningAmbassador()) {
    current_ambassador = current_ambassador->getOwningAmbassador();
  }

  return const_cast<Ambassador*>(current_ambassador);
}

void Ambassador::dump(int level) const {
  if (level > 0) {
    std::cerr << std::endl;
    for (int i = 0; i < level-2; ++i) std::cerr << " ";
    std::cerr << "╘═╕";
  } else {
    std::cerr << "╒";
    for (size_t i = 0; i < 17+getName().size(); ++i) std::cerr << "═";
    std::cerr << std::endl;
    std::cerr << "╞Ambassador name: " << getName() << std::endl;
    std::cerr << "╞";
    for (size_t i = 0; i < 17+getName().size(); ++i) std::cerr << "═";
  }
  std::cerr << std::endl;
  for (const auto &property : getProperties()) {
    for (int i = 0; i < level; ++i) std::cerr << " ";
    std::cerr << "├";
    std::string vtitypename;
    if (dynamic_cast<LinkedProperty*>(property.get()) != nullptr) {
      vtitypename = "linked property";
    } else {
      vtitypename = property->getValueTypeInfo().name();
    }
    std::cerr << property->getName() << " of type " << vtitypename <<  " with value ";
    // no getVTI here because we want to test for Ambassador and no getValueTypeInterface because we do not want to follow links here
    Ambassador *ambassador = dynamic_cast<Ambassador*>(property.get());
    if (ambassador) {
      ambassador->dump(level+2);
    } else {
      try {
        std::cerr << property->toString() << std::endl;
      } catch(const std::runtime_error& error) {
        std::cerr << "(property not applicable for toString operation)" << std::endl;
      }
    }
  }
  if (level > 0) {
    for (int i = 0; i < level-2; ++i) std::cerr << " ";
    std::cerr << "╒═╛" << std::endl;
  }
}
