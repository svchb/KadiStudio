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

#include <QMap>
#include <framework/pluginframework/pluginmanagerinterface.h>

#include <properties/ui/factory/propertywidgetfactory.h>

#include "../widgetfactoryinterface.h"

#include <cpputils/singleton.hpp>

class QWidgetInterface;
class EditDialogInterface;

/**
 * @brief      The widget factory which uses Qt as GUI framework.
 * @ingroup    qwidgetfactory
 *
 */
class QPropertyWidgetFactory : public WidgetFactoryInterface, public PropertyWidgetFactory, public Singleton<QPropertyWidgetFactory> {

  friend class Singleton<QPropertyWidgetFactory>;

  public:
    ~QPropertyWidgetFactory() = default;

    PropertyPanel* createPanel(Ambassador* ambassador) override;
    PropertyPanel* createContainerPanel(AbstractValueTypeInterface* vti, bool issubcontainer) override;

    /**
     * Creates the complete GUI with top panel from a properties model using @p parent as parent widget.
     * After calling this method the parent widget will contain a new scroll area which contains all panels and widgets
     * for all properties in the mapped properties model.
     */
    PropertyPanel* createGui(Ambassador* ambassador, QWidget* parent, const std::set<std::string>& excluded_properties = {}) override;

    /**
     * Creates the complete GUI with top panel from a properties model using @p parent as parent widget.
     * After calling this method the parent widget will contain a new scroll area which contains all panels and widgets
     * for all properties in the mapped properties model.
     */
    PropertyPanel* createScollableGui(Ambassador* ambassador, QWidget* parent, const std::set<std::string>& excluded_properties = {}) override;

  private:
    QPropertyWidgetFactory();

    void finalizeWidget(VTIWidget* vtiwidget) override;

    VTIWidget* createWidgetForVti(AbstractValueTypeInterface* valueTypeInterface) override {
      return PropertyWidgetFactory::createWidgetForVti(valueTypeInterface);
    }
    VTIWidget* createPropertyWidget(Property* property) override {
      return PropertyWidgetFactory::createWidgetForProperty(property);
    }

    QMap<const std::type_info*, QList<EditDialogInterface*>> addondialogs;
    void addDialogToWidget(QWidgetInterface* widget);

    LibFramework::PluginManagerInterface *pluginmanager;

};
