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

#include "workflow.h"

std::unique_ptr<WorkflowInterface> Workflow::create() const {
  return std::make_unique<Workflow>();
}

void Workflow::setFileName(const QString& file_name) {
  this->file_name = file_name;
}

QString Workflow::getFileName() const {
  return file_name;
}

WorkflowState Workflow::getState() const {
  return state;
}

QString Workflow::getStateString() const {
  switch (state) {
    case READY:
      return "Ready";
    case RUNNING:
      return "Running";
    case NEEDS_INTERACTION:
      return "Needs interaction";
    case CANCELLING:
      return "Cancelling";
    case CANCELLED:
      return "Cancelled";
    case ERROR:
      return "Error";
    case FINISHED:
      return "Finished";
    default:
      return "Unknown state";
  }
}

WorkflowState Workflow::stringToState(const QString &stringState) {
  if (stringState == "Ready") {
    return READY;
  } else if (stringState == "Running") {
    return RUNNING;
  } else if (stringState == "Needs_interaction") {
    return NEEDS_INTERACTION;
  } else if (stringState == "Cancelling") {
    return CANCELLING;
  } else if (stringState == "Cancelled") {
    return CANCELLED;
  } else if (stringState == "Error") {
    return ERROR;
  } else if (stringState == "Finished") {
    return FINISHED;
  } else {
    throw std::logic_error("Unknown state string: " + stringState.toStdString());
  }
}

const QDateTime& Workflow::getStartDateTime() const {
  return start_date_time;
}

const QDateTime& Workflow::getEndDateTime() const {
  return end_date_time;
}

QString Workflow::getProcessEngine() const {
  return process_engine;
}

int Workflow::getProgress() const {
  return progress;
}

unsigned int Workflow::getId() const {
  return id;
}

bool Workflow::equals(const WorkflowInterface& rhs) const {
  return id == rhs.getId() &&
         file_name == rhs.getFileName() &&
         state == rhs.getState() &&
         progress == rhs.getProgress() &&
         nodes_processed == rhs.getNodesProcessed() &&
         nodes_processed_in_loops == rhs.getNodesProcessedInLoops() &&
         nodes_total == rhs.getNodesTotal() &&
         start_date_time == rhs.getStartDateTime() &&
         end_date_time == rhs.getEndDateTime() &&
         process_engine == rhs.getProcessEngine();
}

void Workflow::fromJson(QJsonObject json) {
  file_name                = json["fileName"].toString();
  id                       = json["id"].toInt();
  nodes_processed          = json["nodesProcessed"].toInt();
  nodes_processed_in_loops = json["nodesProcessedInLoops"].toInt();
  nodes_total              = json["nodesTotal"].toInt();
  process_engine           = json["processEngine"].toString();
  state                    = stringToState(json["state"].toString());

  progress = (int) ((100.0 / (double) nodes_total) * (double) nodes_processed);

  // parse dates
  const QString datetime_format("HH:mm:ss dd.MM.yyyy");
  QString start_date_time_string = json["startDateTime"].toString();
  start_date_time = QDateTime::fromString(start_date_time_string, datetime_format);
  QString end_date_time_string = json["endDateTime"].toString();
  end_date_time = QDateTime::fromString(end_date_time_string, datetime_format);
}

int Workflow::getNodesProcessed() const {
  return nodes_processed;
}

int Workflow::getNodesProcessedInLoops() const {
  return nodes_processed_in_loops;
}

int Workflow::getNodesTotal() const {
  return nodes_total;
}
