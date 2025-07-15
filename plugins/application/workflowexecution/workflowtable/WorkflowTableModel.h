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
#include <mutex>
#include <unordered_map>
#include <QtCore/QSortFilterProxyModel>

#include <plugins/infrastructure/workflows/processmanager/workflow/workflowinterface.h>

class WorkflowTableModel : public QAbstractTableModel {
public:
  explicit WorkflowTableModel(QObject* parent = nullptr);

  // override functions of QAbstractTableModel:
  int rowCount(const QModelIndex&) const override;
  int columnCount(const QModelIndex&) const override;
  QVariant data(const QModelIndex& index, int role) const override;
  QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
  const WorkflowInterface *getWorkflowAtRow(int row) const;

  void clear();
  int addWorkflow(std::unique_ptr<WorkflowInterface> workflow);
private:
  int rowCountInternal() const;
  const QStringList headerLabels {"Workflow file", "Status", "Progress", "Start", "Ende", "Workflow Engine"};
  std::unordered_map<unsigned int, std::unique_ptr<WorkflowInterface>> workflows;
  std::vector<unsigned int> workflowIndexes; // maps indexes of workflows (unordered map) to the table rows
  std::unique_ptr<std::mutex> mutex;
};
