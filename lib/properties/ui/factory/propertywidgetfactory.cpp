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

#include <properties/data/properties/controls/columnproperty.h>
#include <properties/data/valuetypeinterface/valuetypeinterfacecontainer.h>
#include <properties/data/valuetypeinterface/valuetypeinterfaceiterator.h>
#include <properties/ui/factory/propertywidget.h>
#include <cpputils/string.hpp>

#include "propertypanel.h"
#include "propertywidgetfactory.h"

#include <iostream>
#include <algorithm>


void PropertyWidgetFactory::registerWidget(const std::type_info& type, std::string widget_identifier, CreateWidgetFunc&& create_widget_func) {
  auto widget_list_iterator = registered_widgets.find(type);
  if (widget_list_iterator == registered_widgets.end()) {
    registered_widgets.insert({type, WidgetList()});
    widget_list_iterator = registered_widgets.find(type);
  }
  WidgetList *widget_list = &widget_list_iterator->second;

  if (isWidgetIdentifierRegistered(*widget_list, widget_identifier)) {
    throw std::runtime_error("The widget '" + widget_identifier + "' is already registered");
  }

  widget_list->emplace_back(std::move(widget_identifier), create_widget_func);
}

bool PropertyWidgetFactory::isTypeRegistered(const std::type_info& type) const {
  return (registered_widgets.find(type) != registered_widgets.end());
}

void PropertyWidgetFactory::registerAmbassadorWidget(const std::string& model_name, std::string widget_identifier, CreateWidgetFunc&& create_widget_func) {
  auto widget_list_iterator = registered_ambassador_widgets.find(model_name);
  if (widget_list_iterator == registered_ambassador_widgets.end()) {
    registered_ambassador_widgets.insert({model_name, WidgetList()});
    widget_list_iterator = registered_ambassador_widgets.find(model_name);
  }
  WidgetList *widget_list = &widget_list_iterator->second;

  if (isWidgetIdentifierRegistered(*widget_list, widget_identifier)) {
    throw std::runtime_error("The widget '" + widget_identifier + "' is already registered");
  }

  widget_list->emplace_back(std::move(widget_identifier), create_widget_func);
}


PropertyPanel* PropertyWidgetFactory::createGui(Ambassador* ambassador, const std::set<std::string>& excluded_properties) {
  Controls controls;
  const auto ambassador_hint = ambassador->getHint();

  auto panel = createPanel(ambassador);

  for (const auto &property : ambassador->getProperties()) {
    if (excluded_properties.find(property->getName()) != excluded_properties.end()) {
      continue;
    }

    auto panel_for_property = panel;

    const auto property_hint = property->getHint();

    const auto create_row_or_column = [&](const std::string& group_name) {
      if (controls.find(group_name) == controls.end()) {
        const auto layout_group_key = "layout_group." + group_name;
        const auto is_row = ambassador_hint->hasEntry(layout_group_key) && ambassador_hint->getEntry(layout_group_key) == "row";

        // panel_for_property = is_row ? createRow(panel_for_property) : createColumn(panel_for_property, group_name);
        controls[group_name] = panel_for_property;
      } else {
        panel_for_property = controls.at(group_name);
      }
    };

    if (property_hint->hasEntry("layout_group")) {
      create_row_or_column(property_hint->getEntry("layout_group"));
    } else {
      const auto groups = cpputils::split(property->getName(), ".");
      // The part of the name is the property itself.
      // Traverse the groups and create panels if needed.
      for (size_t i = 0; i < groups.size() - 1; ++i) {
        create_row_or_column(groups.at(i));
      }
    }

    // Properties of type "column" and "row" do only exist to add a simple layout to the GUI.
    auto &property_ref = *property;
    if (property_ref.getValueTypeInfo() == typeid(void)) {
      if (typeid(property_ref) == typeid(ColumnProperty)) {
        panel_for_property->nextColumn();
      } else if (typeid(property_ref) == typeid(RowProperty)) {
        panel_for_property->nextRow();
      }
    }
    // Create a normal widget depending on the property type
    else {
      auto vti_widget = createWidgetForProperty(property.get());
      panel_for_property->addWidget(vti_widget);
    }
  }

  return panel;
}

VTIWidget* PropertyWidgetFactory::createWidgetForProperty(Property* property) {
  auto vti = property->getValueTypeInterface();
  // The property is an ambassador and contains an additional hierarchy that must be traversed.
  if (property->getValueTypeInfo() == Ambassador::getValueTypeInfoStatic()) {
    Ambassador *ambassador = dynamic_cast<Ambassador*>(vti);
    auto modelname = ambassador->getModelName();
    const auto hint = property->getHint();

    // Check if the special widget can be created.
    // Do not fallback to the default creation of a widget for the Ambassador type.
    auto widget_list_iterator = registered_ambassador_widgets.find(modelname);
    if (widget_list_iterator != registered_ambassador_widgets.end()) {
      const auto widget_identifier = hint->getWidgetIdentifier();

      auto widget_list = widget_list_iterator->second;
      auto widget_iterator = std::find_if(widget_list.begin(), widget_list.end(), [&](const auto& widget) {
          std::cout << widget_identifier << " " << widget.first << std::endl;
          return (widget.first == widget_identifier);
        });

      if (widget_iterator != widget_list.end()) {
        std::cout << "is registered widget_identifier " << widget_identifier << std::endl;
        auto widget = widget_iterator->second(vti);
        widget->synchronizeVTI();

        finalizeWidget(widget);

        return widget;
      } else if (not widget_list.empty()) {
        // Try to create the requested widget.
        auto widget = widget_list.front().second(vti);
        widget->synchronizeVTI();

        finalizeWidget(widget);

        return widget;
      }
    }
    const auto panel_name = hint->hasEntry("title") ? hint->getEntry("title") : property->getVisibleName();
    return createGui(ambassador);
  }

  // This property represents just a valuetypeinterface.
  return createWidgetForVti(vti);
}

VTIWidget* PropertyWidgetFactory::createWidgetForVti(AbstractValueTypeInterface* vti, bool isrow, bool issubcontainer) {
  VTIWidget *widget = nullptr;
  // Check if the VTI is a container
  auto container = dynamic_cast<ValueTypeInterfaceContainer*>(vti);

  if (container) {
    auto widget_list_iterator = registered_widgets.find(vti->getValueTypeInfo());
    const auto widget_identifier = vti->getHint()->getWidgetIdentifier();
    if (widget_identifier.empty() || (widget_list_iterator == registered_widgets.end()) || not isWidgetIdentifierRegistered(widget_list_iterator->second, widget_identifier)) {

      auto container_panel = createContainerPanel(vti, issubcontainer);

      for (auto iter = container->begin(); iter != container->end(); iter++) {
        container_panel->addWidget(createWidgetForVti(*iter, !isrow, true));
        (isrow) ? container_panel->nextRow() : container_panel->nextColumn();
      }

      if (issubcontainer) return container_panel;
      widget = container_panel;
    }
  }
  if (widget == nullptr) {

    // Try to create the requested widget.
    widget = createRegisteredWidget(vti);
  }

  widget->synchronizeVTI();

  finalizeWidget(widget);

  return widget;
}

VTIWidget* PropertyWidgetFactory::createRegisteredWidget(AbstractValueTypeInterface* vti) const {
  const auto &type = vti->getValueTypeInfo();

  if (isTypeRegistered(type)) {
    const auto &widgets = registered_widgets.at(type);
    const auto widget_identifier = vti->getHint()->getWidgetIdentifier();

    // Try to find the callable that can create the requested widget.
    const auto iter = std::find_if(widgets.begin(), widgets.end(), [&](const auto& registered_widget) {
      return (registered_widget.first == widget_identifier);
    });

    if (iter != widgets.end()) {
      return iter->second(vti);
    } else if (!widgets.empty()) {
      return widgets.front().second(vti);
    }
  }

  return createDefaultWidget(vti);
}

VTIWidget* PropertyWidgetFactory::createDefaultWidget(AbstractValueTypeInterface* vti) const {
  const auto &string_type = ValueTypeInterface<std::string>::getValueTypeInfoStatic();

  // If there is a string type registered use the first registered widget as fallback.
  if (isTypeRegistered(string_type) && !registered_widgets.at(string_type).empty()) {
    return registered_widgets.at(string_type).front().second(vti);
  }

  // If the default also fails, everything is simply lost.
  throw std::runtime_error(std::string("Type ") + vti->getValueTypeInfo().name() + std::string(" of valueTypeInterface is not supported."));
}

bool PropertyWidgetFactory::isWidgetIdentifierRegistered(const WidgetList& widget_list, const std::string& widget_identifier) const {
  return std::find_if(widget_list.begin(), widget_list.end(), [&](const auto& widget) {
    return (widget.first == widget_identifier);
  }) != widget_list.end();
}
