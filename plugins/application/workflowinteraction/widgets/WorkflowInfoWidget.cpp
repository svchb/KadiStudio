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

#include <QLabel>
#include <QGridLayout>
#include <QtWidgets/QGroupBox>
#include <framework/enhanced/qbucketprogressbar.h>

#include "WorkflowInfoWidget.h"

WorkflowInfoWidget::WorkflowInfoWidget(LibFramework::PluginManagerInterface* pluginManager)
    : QWidget(), workflowPluginManager(pluginManager), workflowFileLabel(new QLabel()), workflowStateLabel(new QLabel()),
      workflowProgressInfoLabel(new QLabel()), workflow_progressbar(new QBucketProgressBar()) {

  auto layout = new QHBoxLayout();
  setLayout(layout);

  workflow_progressbar->setMinimumWidth(300);
  workflow_progressbar->setMaximumWidth(300);
  workflow_progressbar->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Minimum);
  workflow_progressbar->setColor(0, QColor("#80c342"));
  workflow_progressbar->setValue(0, 0);

  workflow_progressbar->addBucket(0, 100);
  workflow_progressbar->setColor(1, QColor("#b1da8b"));
  workflow_progressbar->setValue(1, 0);

  layout->addWidget(new QLabel("File:"));
  layout->addWidget(workflowFileLabel);
  layout->addStretch();
  layout->addWidget(new QLabel("State:"));
  layout->addWidget(workflowStateLabel);
  layout->addStretch();
  layout->addWidget(new QLabel("Progress:"));
  layout->addWidget(workflow_progressbar);
  layout->addWidget(workflowProgressInfoLabel);
}

void WorkflowInfoWidget::setWorkflow(WorkflowInterface* workflow) {
  WorkflowInfoWidget::workflow = workflow;

  workflow_progressbar->setValue(0, 0);
  workflow_progressbar->setValue(1, 0);
  workflow_progressbar->setMaximum(0, workflow->getNodesTotal() - workflow->getNodesProcessedInLoops());
  workflow_progressbar->setMaximum(1, workflow->getNodesProcessedInLoops());
}

void WorkflowInfoWidget::updateInfo() {
  if (workflow) {
    workflowStateLabel->setText(workflow->getStateString());
    updateStatusLabelStyle(workflow->getState());
    if (workflow->getState() != READY) {
      QString progress_text = QString::number(workflow->getNodesProcessed());
      progress_text += QString("/%1 nodes processed").arg(workflow->getNodesTotal());
      if (workflow->getNodesProcessedInLoops() > 0) {
        progress_text += QString(" (%1 in loops)").arg(workflow->getNodesProcessedInLoops());
      }
      workflowProgressInfoLabel->setText(progress_text);
    } else {
      workflowProgressInfoLabel->setText("");
    }
    updateProgressBar();
    workflowFileLabel->setText(workflow->getFileName());
  }
}

void WorkflowInfoWidget::updateStatusLabelStyle(WorkflowState state) {
  switch (state) {
    case RUNNING:
      workflowStateLabel->setStyleSheet("QLabel { background-color: none; color: green;}");
      break;
    case NEEDS_INTERACTION:
      workflowStateLabel->setStyleSheet("QLabel { background-color: yellow; color: text;}");
      break;
    case ERROR:
      workflowStateLabel->setStyleSheet("QLabel { background-color: red; color: black;}");
      break;
    case FINISHED:
      workflowStateLabel->setStyleSheet("QLabel { background-color: #80c342; color: black;}");
      break;
    case CANCELLING:
      workflowStateLabel->setStyleSheet("QLabel { background-color: #C38888; color: black;}");
      break;
    case CANCELLED:
      workflowStateLabel->setStyleSheet("QLabel { background-color: gray; color: black;}");
      break;
    default:
      workflowStateLabel->setStyleSheet("QLabel { background-color: none; color: black;}");
  }
}

void WorkflowInfoWidget::updateProgressBar() {
  workflow_progressbar->setMaximum(0, workflow->getNodesTotal() - workflow->getNodesProcessedInLoops());
  workflow_progressbar->setMaximum(1, workflow->getNodesProcessedInLoops());
  workflow_progressbar->setValue(0, workflow->getNodesProcessed());
  workflow_progressbar->setValue(1, workflow->getNodesProcessedInLoops());
}
