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

#include "workflownode.h"
#include "plugins/application/workfloweditor/domain/tool.h"
#include "plugins/application/workfloweditor/domain/executionprofile.h"

using QtNodes::NodeDataType;
using QtNodes::PortType;
using QtNodes::PortIndex;

class ExecutionProfileChooser;
using ExecutionProfiles::ExecutionProfile;


/**
 * @brief      A dynamic node model which represents a tool based on its XML
 *             description (xmlhelp), for instance has an input port for each
 *             <param> of the tool.
 * @ingroup    models
 */
class ToolNode : public WorkflowNode {
  Q_OBJECT

  public:
    ToolNode();

    QString caption() const override;
    QString name() const override;
    unsigned int nPorts(PortType portType) const override;
    NodeDataType dataType(PortType portType, PortIndex portIndex) const override;
    void setTool(std::unique_ptr<Tool> tool);
    QString portCaption(PortType portType, PortIndex portIndex) const override;
    ConnectionPolicy portConnectionPolicy(PortType portType, PortIndex) const override;

    QJsonObject save() const override;
    void load(QJsonObject const &p) override;

    bool init() override;

    virtual QWidget* cornerWidget();
    void setExecutionProfile(ExecutionProfile executionProfile);
    ExecutionProfile getExecutionProfile() const;

  protected:
    std::unique_ptr<Tool> tool;

  private:
    QString modelName;

    void setNodeStyleBasedOnProfile();

    ExecutionProfile executionProfile;
    ExecutionProfileChooser *executionProfileChooser;

    static NodeDataType typeNameToType(const QString& typeName);
};
