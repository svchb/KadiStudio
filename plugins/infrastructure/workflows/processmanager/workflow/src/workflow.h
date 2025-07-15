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
#include <QString>
#include <QDateTime>
#include <QJsonObject>
#include "../workflowinterface.h"
#include "../workflowstate.h"

/**
 * @brief      A class to represent instances of workflow execution
 * @ingroup    workflow
 */
class Workflow : public WorkflowInterface {

public:
  Workflow() : id(0), state(READY), progress(0), nodes_processed(0), nodes_processed_in_loops(0), nodes_total(0) {};

  std::unique_ptr<WorkflowInterface> create() const override;
  unsigned int getId() const override;

  void setFileName(const QString& file_name) override;
  QString getFileName() const override;

  WorkflowState getState() const override;
  QString getStateString() const override;
  const QDateTime& getStartDateTime() const override;
  const QDateTime& getEndDateTime() const override;
  QString getProcessEngine() const override;
  int getProgress() const override;
  int getNodesProcessed() const override;
  int getNodesProcessedInLoops() const override;
  int getNodesTotal() const override;

  void fromJson(QJsonObject json) override;

  bool equals(const WorkflowInterface& rhs) const override;

private:

  static WorkflowState stringToState(const QString& string);

  unsigned int id;
  QString file_name;
  WorkflowState state;
  int progress;
  int nodes_processed;
  int nodes_processed_in_loops;
  int nodes_total;
  QDateTime start_date_time;
  QDateTime end_date_time;
  QString process_engine;
};
