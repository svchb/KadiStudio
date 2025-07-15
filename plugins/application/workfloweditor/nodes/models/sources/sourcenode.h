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

#include <QtCore/QObject>

#include "plugins/application/workfloweditor/nodes/data/genericnodedata.h"
#include "plugins/application/workfloweditor/nodes/models/workflownode.h"


/**
 * @brief      Abstract super class for all source node models
 * @ingroup    sources
 */
class SourceNode : public WorkflowNode {
  public:
    SourceNode();
    virtual ~SourceNode() = default;

    // default implementations
    unsigned int nPorts(PortType portType) const override;
    bool captionVisible() const override;
    QJsonObject save() const override;

  protected:
    /**
     * Set a value and update the widget
     * @param _value
     */
    virtual void setValue(const QString& _value) = 0;

    /**
     * Return the value that the SourceNode should have when added to the workflow (or restoring fails)
     * @return
     */
    virtual QString getDefaultValue() const = 0;

    std::unique_ptr<GenericNodeData> value;
};
