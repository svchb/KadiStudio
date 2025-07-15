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

#include <memory>

#include "InteractionWidget.h"
#include <plugins/infrastructure/qpropertywidgetfactory/widgetfactoryinterface.h>
#include <plugins/infrastructure/workflows/processmanager/processmanagerinterface.h>

InteractionWidget::InteractionWidget(LibFramework::PluginManagerInterface *pluginManagerInterface, QWidget *parent)
  : QTabWidget(parent) {
  if (pluginManagerInterface) {
    widgetFactoryInterface = pluginManagerInterface->getInterface<WidgetFactoryInterface*>("/plugins/infrastructure/qpropertywidgetfactory");
    processmanager_interface = pluginManagerInterface->getInterface<ProcessManagerInterface*>("/plugins/infrastructure/workflows/processmanager");
  }
}

void InteractionWidget::updateView() {
  for (const auto& interactionPage : interactionPages) {
    interactionPage.second->updateView();
  }
  switchToRightmostPage();
}

void InteractionWidget::addInteraction(InteractionInterface* interaction) {
  int pageNumber = interaction->getPageNumber();
  if (pageNumber < 0) {
    qDebug() << "Ignoring interaction with invalid pageNumber " << pageNumber;
    return;
  }
  InteractionPageWidget *pageWidget = findOrCreatePageWidget(pageNumber);
  if (pageWidget) {
    pageWidget->addInteraction(interaction);
  } else {
    qDebug() << "Unable to find or create interaction page " << pageNumber << " to add the interaction";
  }
}

bool InteractionWidget::sendValues() {
  for (const auto& interactionPage : interactionPages) {
    if (!interactionPage.second->sendValues()) {
      return false;
    }
  }
  return true;
}

void InteractionWidget::setWorkflowId(unsigned int id) {
  InteractionWidget::workflowId = id;
}

bool InteractionWidget::formValid(std::vector<std::string>& invalid_fields) {
  for (const auto& interactionPage : interactionPages) {
    if (!interactionPage.second->formValid(invalid_fields)) {
      return false;
    }
  }
  return true;
}

InteractionPageWidget *InteractionWidget::findOrCreatePageWidget(int pageNumber) {
  if (pageNumber < 0) return nullptr;
  if (interactionPages.find(pageNumber) == interactionPages.end()) {
    for (const auto& interactionPage : interactionPages) {
      // disable all previous pages before adding the new one
      if (interactionPage.first < pageNumber) {
        interactionPage.second->setPageDisabled(true);
      }
    }
    interactionPages[pageNumber] = std::make_unique<InteractionPageWidget>(widgetFactoryInterface, processmanager_interface, workflowId, this);

    /* Create empty pages in between, to assure that the pages will be added in the correct order.
     * This way, we don't need to reorder the tabs in the TabWidget afterwards. */
    findOrCreatePageWidget(pageNumber - 1);

    addTab(interactionPages[pageNumber].get(), "Form " + QString::number(pageNumber + 1));
  }
  return interactionPages[pageNumber].get();
}

void InteractionWidget::addInteractions(std::vector<InteractionInterface*> interactions) {
  std::sort(interactions.begin(), interactions.end(), [](const auto& lhs, const auto& rhs) {
    return lhs->getOrder() < rhs->getOrder();
  });
  for (auto interaction : interactions) {
    addInteraction(interaction);
  }
}

void InteractionWidget::setWidgetsDisabled(bool disabled) {
  for (const auto& interactionPage : interactionPages) {
    interactionPage.second->setPageDisabled(disabled);
  }
}

void InteractionWidget::switchToRightmostPage() {
  setCurrentIndex(count() - 1);
}

void InteractionWidget::reset() {
  interactionPages.clear();
  clear();
}
