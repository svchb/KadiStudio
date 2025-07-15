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

#include <QDateTime>
#include <QJsonObject>
#include <framework/pluginframework/pluginclientinterface.h>
#include <memory>
#include "workflowstate.h"

/**
 * @brief      Provides access to a workflow object.
 * @ingroup    processmanager
 */
class WorkflowInterface : public LibFramework::PluginClientInterface {

  public:
    ~WorkflowInterface() override = default;
    virtual std::unique_ptr<WorkflowInterface> create() const = 0;
    virtual unsigned int getId() const = 0;
    virtual void setFileName(const QString &fileName) = 0;
    virtual QString getFileName() const = 0;
    virtual WorkflowState getState() const = 0;
    virtual QString getStateString() const = 0;
    virtual const QDateTime& getStartDateTime() const = 0;
    virtual const QDateTime& getEndDateTime() const = 0;
    virtual QString getProcessEngine() const = 0;
    virtual int getProgress() const = 0;
    virtual int getNodesProcessed() const = 0;
    virtual int getNodesProcessedInLoops() const = 0;
    virtual int getNodesTotal() const = 0;
    virtual void fromJson(QJsonObject json) = 0;

    virtual bool equals(const WorkflowInterface &rhs) const = 0;

    bool operator==(const WorkflowInterface &rhs) const {
      return equals(rhs);
    }
    bool operator!=(const WorkflowInterface &rhs) const {
      return !equals(rhs);
    }
};
