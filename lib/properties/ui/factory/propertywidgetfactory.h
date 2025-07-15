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

#include <functional>
#include <set>
#include <string>
#include <typeindex>
#include <unordered_map>

class AbstractValueTypeInterface;
class Property;
class Ambassador;
class PropertyPanel;
class VTIWidget;


/// @brief Factory that creates GUI-Elements based on properties.
///
/// Supports construction complete GUI panels and not only the creation of widgets for properties.
/// It uses a property's domain and ambassador hierarchy to determine in which panel a property widget has to appear.
/// It then creates a corresponding widget to the property with the specified backend.
/// Creating a new GUI backend can be done by inheriting this class and registering functions to create widgets based on types and names.
/// For widgets which correspond to properties you have to derive from @ref PropertyWidget.
/// For custom panels you have to derive from @ref PropertyPanel.
class PropertyWidgetFactory {
  public:
    /// @brief Stores panels by their domain.
    /// The key is the complete domain of the panel.
    /// The value is the panel instance.
    using Controls = std::unordered_map<std::string, PropertyPanel*>;

    /// @brief Callable that is registered for the creation of widget.
    using CreateWidgetFunc = std::function<VTIWidget*(AbstractValueTypeInterface *vti)>;

    /// @brief Stores a list of widget identifiers together with the function to create a widget for it.
    using WidgetList = std::vector<std::pair<std::string, CreateWidgetFunc>>;

    /// @brief Destroys the factory.
    virtual ~PropertyWidgetFactory() = default;

    virtual PropertyPanel* createPanel(Ambassador* ambassador) = 0;
    virtual PropertyPanel* createContainerPanel(AbstractValueTypeInterface* vti, bool issubcontainer) = 0;

    /// @brief Automatically creates a whole GUI for the given ambassador.
    /// @param ambassador The ambassador.
    /// @param excluded_properties Names of properties that should be excluded.
    /// @return The panel with all created widgets.
    PropertyPanel* createGui(Ambassador* ambassador, const std::set<std::string>& excluded_properties = {});

    /// @brief Create a widget for a property.
    /// @param property The property.
    /// @return The widget.
    VTIWidget* createWidgetForProperty(Property* property);

    /// @brief Create a widget for a VTI.
    /// @param vti The vti.
    /// @return The widget.
    VTIWidget* createWidgetForVti(AbstractValueTypeInterface* vti, bool isrow = true, bool issubcontainer = false);

  protected:
    /// @brief Registers a widget for a type and name.
    /// @param type The underlying type of the property.
    /// @param widget_identifier The widget identifier.
    /// @param create_widget_func The callable to create the widget.
    void registerWidget(const std::type_info& type, std::string widget_identifier, CreateWidgetFunc&& create_widget_func);

    /// @brief Registers a widget for a type and name.
    /// @tparam T The underlying type of the property.
    /// @param widget_identifier The widget identifier.
    /// @param create_widget_func The callable to create the widget.
    template<typename T>
    void registerWidget(std::string widget_identifier, CreateWidgetFunc&& create_widget_func) {
      registerWidget(typeid(T), std::move(widget_identifier), std::move(create_widget_func));
    }

    /// @brief Checks if a type is registered.
    /// @param type The type.
    /// @return True if the type is registered.
    bool isTypeRegistered(const std::type_info& type) const;

    /// @brief Checks if a type with a widget identifier is registered.
    /// @param widget_list list with the registered widget of a property type.
    /// @param widget_identifier The identifier of the widget.
    /// @return True if type with widget identifier is registered.
    bool isWidgetIdentifierRegistered(const WidgetList& widget_list, const std::string& widget_identifier) const;

    /// @brief Creates a widget based on the registered widgets.
    /// @param vti The VTI to create the widget for.
    /// @return The widget.
    VTIWidget* createRegisteredWidget(AbstractValueTypeInterface* vti) const;

    /// @brief Creates a default widget for a VTI.
    /// @param vti The VTI to create the widget for.
    /// @return The widget.
    VTIWidget* createDefaultWidget(AbstractValueTypeInterface* vti) const;

    /// @brief Creates a default widget for a VTI.
    /// @param model_name
    /// @param widget_identifier The identifier of the widget.
    /// @param create_widget_func The callable to create the widget.
    void registerAmbassadorWidget(const std::string& model_name, std::string widget_identifier, CreateWidgetFunc&& create_widget_func);

    /// @brief Allows a derived factory to finalize the widget after the creation.
    /// @param vtiwidget The widget.
    virtual void finalizeWidget(VTIWidget* vtiwidget) = 0;

  protected:
    std::unordered_map<std::type_index, WidgetList> registered_widgets;
    std::unordered_map<std::string, WidgetList>     registered_ambassador_widgets;
};
