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

#include <algorithm>

#include "WorkflowTableModel.h"

WorkflowTableModel::WorkflowTableModel(QObject *parent) : QAbstractTableModel(parent),
                                                          mutex(std::make_unique<std::mutex>()) {

}

int WorkflowTableModel::rowCount(const QModelIndex&) const {
  // the parent model index is only relevant on hierarchical models (like for tree views)
  return rowCountInternal();
}

int WorkflowTableModel::columnCount(const QModelIndex&) const {
  // the parent model index is only relevant on hierarchical models (like for tree views)
  return headerLabels.size();
}

QVariant WorkflowTableModel::data(const QModelIndex &index, int role) const {
  if (role != Qt::DisplayRole) {
    return QVariant();
  }
  if (index.row() >= 0) {
    auto row = (unsigned int) index.row();
    if (row < workflowIndexes.size()) {
      unsigned int workflowIndex = workflowIndexes[row];
      if (workflows.find(workflowIndex) != workflows.end()) {
        switch (index.column()) {
          case 0:
            return workflows.at(workflowIndex)->getFileName();
          case 1:
            return (workflows.at(workflowIndex)->getStateString());
          case 2:
            return QString::number(workflows.at(workflowIndex)->getProgress());
          case 3:
            return workflows.at(workflowIndex)->getStartDateTime();
          case 4:
            return workflows.at(workflowIndex)->getEndDateTime();
          case 5:
            return workflows.at(workflowIndex)->getProcessEngine();
        }
      }
    }
  }
  return QVariant();
}

QVariant WorkflowTableModel::headerData(int section, Qt::Orientation orientation, int role) const {
  if (orientation != Qt::Horizontal || role != Qt::DisplayRole) return {};
  return headerLabels[section];
}

int WorkflowTableModel::addWorkflow(std::unique_ptr<WorkflowInterface> workflow) {
  // the table model is responsible for notifying it's clients about changes to the data
  // instead of emitting the signals of QAbstractTableModel, one should use the corresponding
  // functions beginInsertRows(), endInsertRows(), etc. which internally emit the signals

  int result = 0;
  if (!mutex) throw std::logic_error("Can not lock resources");
  mutex->lock();

  unsigned int workflowId = workflow->getId();
  if (workflows.find(workflowId) != workflows.end()) {
    // workflow with this id already existing - update if changed

    auto iter = std::find(workflowIndexes.begin(), workflowIndexes.end(), workflowId);
    int mappedIndex = std::distance(workflowIndexes.begin(), iter);
    if (*workflows[workflowId] != *workflow) {

      workflows[workflowId] = std::move(workflow);
      emit dataChanged(createIndex(mappedIndex, 0), createIndex(mappedIndex, 0));
    }
    result = mappedIndex;
  } else {
    // workflow not existing, insert it
    // map the new index to a row
    workflowIndexes.push_back(workflowId);
    int newIndex = (int) workflowIndexes.size() - 1;
    beginInsertRows(QModelIndex(), newIndex, newIndex);
    workflows[workflowId] = std::move(workflow);
    endInsertRows();
    emit dataChanged(createIndex(newIndex, 0), createIndex(newIndex, 0));

    result = newIndex;
  }

  mutex->unlock();
  return result;
}

void WorkflowTableModel::clear() {
  beginResetModel();
  workflows.clear();
  workflowIndexes.clear();
  endResetModel();
}


int WorkflowTableModel::rowCountInternal() const {
  return workflows.size();
}

const WorkflowInterface *WorkflowTableModel::getWorkflowAtRow(int row) const {
  WorkflowInterface *result = nullptr;
  if (row < rowCountInternal() && row >= 0) {
    unsigned int index = workflowIndexes[row];
    if (workflows.find(index) != workflows.end()) {
      result = workflows.at(index).get();
    }
  }
  return result;
}
