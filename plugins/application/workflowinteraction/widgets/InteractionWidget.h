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

#include <QTabWidget>
#include <QDebug>
#include <memory>
#include <map>

#include "../WorkflowInteraction.h"
#include "InteractionPageWidget.h"

class InteractionWidget : public QTabWidget {
  Q_OBJECT

public:
  explicit InteractionWidget(LibFramework::PluginManagerInterface *pluginManagerInterface, QWidget *parent = Q_NULLPTR);

  void setWorkflowId(unsigned int workflowId);
  void addInteractions(std::vector<InteractionInterface*> interactions);
  void addInteraction(InteractionInterface* interaction);
  void updateView();
  bool sendValues();
  bool formValid(std::vector<std::string>& invalid_fields);
  void switchToRightmostPage();

  void reset();

public Q_SLOTS:
  void setWidgetsDisabled(bool disabled);

private:
  WidgetFactoryInterface *widgetFactoryInterface;
  ProcessManagerInterface *processmanager_interface;
  InteractionPageWidget* findOrCreatePageWidget(int pageNumber);
  unsigned int workflowId;
  std::map<int, std::unique_ptr<InteractionPageWidget>> interactionPages;
};
