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

#include <QWidget>
class QLabel;

#include <framework/pluginframework/pluginmanagerinterface.h>
#include <plugins/infrastructure/workflows/processmanager/workflow/workflowinterface.h>

class QBucketProgressBar;

class WorkflowInfoWidget : public QWidget {
  Q_OBJECT

public:
  WorkflowInfoWidget(LibFramework::PluginManagerInterface* pluginManager);

  void setWorkflow(WorkflowInterface* workflow);
  void updateInfo();

private:
  void updateStatusLabelStyle(WorkflowState state);
  void updateProgressBar();

  LibFramework::PluginManagerInterface *workflowPluginManager{};
  WorkflowInterface *workflow{};

  QLabel *workflowFileLabel{};
  QLabel *workflowStateLabel{};
  QLabel *workflowProgressInfoLabel{};
  QBucketProgressBar *workflow_progressbar;

};
