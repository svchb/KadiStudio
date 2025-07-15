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

#include "ambassador.h"


/**
  * @brief  Interface for container that are made up of properties.
  *
  *         It provides functionality to add, get, and modify properties.
  */
class PropertiesModel : public Ambassador {
  public:
    /**
     * @brief C'tor
     *
     *
     * @param modelname The name of this model. Has to be unique inside a model hierarchy
     * @param name The unique name of this model.
     *
     * @note The optional <b>type</b> parameter exists to add some semantic to the model.
     *       While the name serves as an id, the type can be used to define groups of models.
     */
    PropertiesModel(const std::string& modelname, const std::string& name) : Ambassador(modelname, name) {}
    PropertiesModel(const std::string& name) : PropertiesModel(name, name) {};
    ~PropertiesModel() = default;

    /**
      * @brief Adds a property to this model. Replaces a property with the same name.
      *
      * @param property The property to add
      *
      * @return The added property.
      */
    Property* addProperty(Property* property);

    /**
     * Add a new property to the internal model. Return a pointer to the added property.
     * Warning: Ownership will be transferred to the PropertiesModel implicitly.
     * @param property deletable pointer to new property
     * @return pointer to the added property (@param property)
     */
    template<typename T>
    T* addProperty(T* property) {
      static_assert(
        std::is_base_of<Property, T>::value,
        "T must be a descendant of Property"
      );
      addProperty(static_cast<Property*>(property));
      return property;
    }

    /**
     * @brief Removes and deletes a property from this model.
     *
     * @param name The name of property to remove.
     */
    void removeProperty(const std::string& name);

};
