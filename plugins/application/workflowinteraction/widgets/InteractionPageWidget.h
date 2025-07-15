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

#include <memory>
#include <QtWidgets/QWidget>

#include <properties/data/property.h>
#include <properties/ui/factory/propertypanel.h>

class WidgetFactoryInterface;
class InteractionInterface;
class ProcessManagerInterface;
class PropertiesModel;

class InteractionPageWidget : public QWidget {
Q_OBJECT

public:
  InteractionPageWidget(WidgetFactoryInterface* widgetFactoryInterface,
                        ProcessManagerInterface* processmanager_interface,
                        unsigned int workflowId,
                        QWidget *parent = Q_NULLPTR);

  QWidget* getRootWidget() const;
  void addInteraction(InteractionInterface* interaction);
  bool formValid(std::vector<std::string>& invalid_fields);
  void updateView();
  bool sendValues();
  void setPageDisabled(bool disabled);

  void clear();

private:
  void addProperty(Property* property);

  unsigned int workflowId;
  WidgetFactoryInterface *widgetFactoryInterface;
  ProcessManagerInterface *processmanager_interface;
  QWidget *rootWidget;

  bool viewInitialized;
  bool needsRefresh; // true if new interactions were added after the last view update

  /* property related */
  std::unique_ptr<PropertyPanel> propertyPanel; // when deleted, deletes QtPropertyPanel -> PropertyWidgets
  PropertiesModel *propertyModel; // owned by the ambassador
  std::vector<Property*> properties;

};
