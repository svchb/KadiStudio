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

#include <stdexcept>
#include <string>
#include <typeinfo>

#include "properties/data/valuetypeinterface/abstractvaluetypeinterface.h"

class Ambassador;
class PersistentManager;

/**
 * @brief      A property of a datamodel.
 * @ingroup    data
 *
 * The Property class is the base of all property structures.
 * It does not have a type on its own, so there is no template parameter.
 * The Property itself represents a named value.
 *
 * Property must be combined with ValueType<T> to form a triplet of value, type and name.
 */
class Property {

  public:

    virtual ~Property() = default;

    bool hasName(const std::string& name) const;
    const std::string& getName() const;

    bool makesDirty() const;

    virtual void store(PersistentManager* persistentmanager, const std::string& keynamespace) const;
    virtual void load(PersistentManager* persistentmanager, const std::string& keynamespace);

    /**
     * Returns a ValueTypeInterface<T>* as AbstractValueTypeInterface* (to be independend of T).
     * ValueTypeInterface<T> is bound to a specific type and provides funtions to get/set a value.
     */
    virtual AbstractValueTypeInterface* getValueTypeInterface() = 0;
    virtual const AbstractValueTypeInterface* getValueTypeInterface() const = 0;

    virtual const std::type_info& getValueTypeInfo() const {
      return getValueTypeInterface()->getValueTypeInfo();
    }

    const ValueTypeInterfaceHint* getHint() const {
      return getValueTypeInterface()->getHint();
    }

    ValueTypeInterfaceHint* updateHint() {
      return getValueTypeInterface()->updateHint();
    }

    virtual std::string toString() const {
      return getValueTypeInterface()->toString();
    }
    virtual void fromString(const std::string& str) {
      getValueTypeInterface()->fromString(str);
    }

    Ambassador* getOwningAmbassador() const {
      return ambassador;
    }

    const std::string getVisibleName() const;

  protected:
    Property(const std::string& name, bool makesdirty)
        : name(name), makesdirty(makesdirty), ambassador(nullptr) {
      if (name.empty()) throw std::runtime_error("Property must have a name");
    }

  private:
    std::string name;
    bool makesdirty;

    void setOwningAmbassador(Ambassador* ambassador);

    Ambassador *ambassador;

    friend class PropertiesModel;
    friend class Ambassador;
};
