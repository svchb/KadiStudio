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

#include <QGridLayout>
#include <QMessageBox>
#include <plugins/infrastructure/qpropertywidgetfactory/widgetfactoryinterface.h>
#include <plugins/infrastructure/qpropertywidgetfactory/qwidgetinterface.h>
#include <plugins/infrastructure/workflows/processmanager/processmanagerinterface.h>
#include "../properties/PropertyParser.h"

#include "InteractionPageWidget.h"

InteractionPageWidget::InteractionPageWidget(WidgetFactoryInterface *widgetFactoryInterface,
                                             ProcessManagerInterface* processmanager_interface,
                                             unsigned int workflowId,
                                             QWidget *parent)
  : QWidget(parent), workflowId(workflowId), widgetFactoryInterface(widgetFactoryInterface),
    processmanager_interface(processmanager_interface), viewInitialized(false), needsRefresh(false),
    propertyPanel(nullptr), propertyModel(new PropertiesModel("Interactions")) {

  rootWidget = new QWidget(); // following the qt docu: do not set a parent for the page widgets
  auto pageLayout = new QGridLayout();
  rootWidget->setLayout(pageLayout);
  rootWidget->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored); // unsure if this is necessary
  auto layout = new QGridLayout();
  layout->addWidget(rootWidget);
  setLayout(layout);
}

QWidget *InteractionPageWidget::getRootWidget() const {
  return rootWidget;
}

void InteractionPageWidget::addProperty(Property *property) {
  if (!propertyModel->hasProperty(property->getName())) {
    propertyModel->addProperty(property);
    if (viewInitialized) {
      needsRefresh = true;
    }
  }
  // note: this does not update values for existing properties for now
  // (could make sense if there are multiple client processes involved?)
}

bool InteractionPageWidget::sendValues() {
  for (const auto &property : propertyModel->getProperties()) {
    // and which are on a page which is not disabled
    QString propertyValue = PropertyParser::valueFromProperty(property.get());
    QString interactionId = QString::fromStdString(property->getName());

    const ValueTypeInterfaceHint *hint = property->getHint();
    if (hint->getEntry("direction") == "output") {
      continue;
    }
    if (!propertyValue.isEmpty()) {
      try {
        processmanager_interface->inputValue(workflowId, interactionId, propertyValue);
      } catch (std::exception &exception) {
        QMessageBox::critical(this, tr("Error"), tr("Could not send user input to process engine:") + "\n" + exception.what());
        return false;
      }
    }
  }
  return true;
}

void InteractionPageWidget::updateView() {
  if (!viewInitialized || needsRefresh) {
    if (propertyPanel) {
      propertyPanel.reset();
    }
    if (!widgetFactoryInterface) throw std::logic_error(tr("Unable to access plugin to generate widgets").toStdString());
    propertyPanel = std::unique_ptr<PropertyPanel>(widgetFactoryInterface->createGui(propertyModel, rootWidget));
    rootWidget->layout()->addWidget(dynamic_cast<QWidgetInterface*>(propertyPanel.get())->getWidget());
    viewInitialized = true;
    needsRefresh = false;
  }
}

bool InteractionPageWidget::formValid(std::vector<std::string>& invalid_fields) {
  return PropertyParser::validatePropertyValues(propertyModel, invalid_fields);
}

void InteractionPageWidget::addInteraction(InteractionInterface* interaction) {
  Property *result = PropertyParser::buildProperty(interaction);
  addProperty(result);
}

void InteractionPageWidget::setPageDisabled(bool disabled) {
  if (propertyPanel) {
    propertyPanel->setDisabled(disabled);
  }
}

void InteractionPageWidget::clear() {
  propertyPanel.reset();

}
