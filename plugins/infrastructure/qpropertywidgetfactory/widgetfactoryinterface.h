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

#include <framework/pluginframework/pluginclientinterface.h>

#include <properties/data/ambassador.h>
#include <properties/ui/factory/vtiwidget.h>

class Property;
class PropertyPanel;
class QWidget;


/**
 * @brief      Provides an interface for the plugins to interact with the widget factory.
 * @ingroup    framework
 */
class WidgetFactoryInterface : public LibFramework::PluginClientInterface {

  public:

    virtual ~WidgetFactoryInterface() = default;

    template<typename T>
    T* createWidget(AbstractValueTypeInterface* valueTypeInterface) {
      return dynamic_cast<T*>(createWidgetForVti(valueTypeInterface));
    }

    template<typename T>
    T* createWidget(Property* property) {
      return dynamic_cast<T*>(createPropertyWidget(property));
    }

    virtual PropertyPanel* createGui(Ambassador* ambassador, QWidget* parent, const std::set<std::string>& excluded_properties = {}) = 0;

    virtual PropertyPanel* createScollableGui(Ambassador* ambassador, QWidget* parent, const std::set<std::string>& excluded_properties = {}) = 0;

  private:

    virtual VTIWidget* createWidgetForVti(AbstractValueTypeInterface* valueTypeInterface) = 0;
    virtual VTIWidget* createPropertyWidget(Property* property)                           = 0;

};
