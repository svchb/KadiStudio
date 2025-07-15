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

#include "../../data/ambassador.h"
#include "vtiwidget.h"


/**
 * @brief      Base class for all property widgets.
 * @ingroup    ui
 */
class PropertyWidget : public VTIWidget {

  public:
    virtual ~PropertyWidget() = default;

  protected:
    /**
     * Constructor for PropertyWidget, provide a valid PropertyValueTypeInterface pointer which can be used by the widget
     * to get and set the property value (or element value of a ContainerProperty).
     *
     * @param valuetypeinterface Pointer of type AbstractValueTypeInterface
     */
    explicit PropertyWidget(Property* property)
        : VTIWidget(property->getValueTypeInterface()) {
    }

    Property* getProperty() const {
      return dynamic_cast<Property*>(valuetypeinterface);
    }

    /// getProperty<T> must be called with T=Ambassador*, not T=const Ambassador*
    template<typename TProperty>
    TProperty* getProperty() const {
      static_assert(
        std::is_base_of<Property, TProperty>::value,
        "TProperty must be a descendant of Property"
      );

      TProperty *tproperty = dynamic_cast<TProperty*>(getProperty());
#ifdef DEBUG
      if (tproperty == nullptr) {
        throw std::runtime_error("Property '" + getProperty()->getName() + "' is not of type " + typeid(TProperty).name() + ".");
      }
#endif

      return tproperty;
    }

    /**
     * For ControlProperties (with T=Ambassador*), this allows to get a pointer to the internal ambassador
     * (which is the actual property value in this case).
     * @return Ambassador* to the inner ambassador if applicable, or nullptr otherwise
     */
    Ambassador* getInnerAmbassador() const {
      return dynamic_cast<Ambassador*>(valuetypeinterface);
    }

};
