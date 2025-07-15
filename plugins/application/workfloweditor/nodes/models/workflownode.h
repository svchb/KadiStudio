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

#include <QtNodes/NodeDelegateModel>
#include <QtNodes/AbstractNodePainter>
#include "../../config.h"

using QtNodes::NodeDelegateModel;
using QtNodes::NodeDataType;
using QtNodes::NodeData;
using QtNodes::PortType;
using QtNodes::PortIndex;
using QtNodes::ConnectionPolicy;

/**
 * @brief      A superclass for all node models, which adjusts node connection
 *             policy settings, node style, etc. globally.
 * @ingroup    models
 */
class WorkflowNode : public NodeDelegateModel {

  public:
    WorkflowNode();
    ~WorkflowNode() override = default;

    // abstract functions to suffice super class
    QString caption() const override = 0;
    QString name() const override = 0;
    unsigned int nPorts(PortType portType) const override = 0;
    NodeDataType dataType(PortType portType, PortIndex portIndex) const override = 0;

    // default implementations
    void setInData(std::shared_ptr<NodeData> data, PortIndex portIndex) override;
    std::shared_ptr<NodeData> outData(PortIndex port) override;
    QString portCaption(PortType, PortIndex) const override;
    bool portCaptionVisible(PortType, PortIndex) const override { return true; }
    ConnectionPolicy portConnectionPolicy(PortType portType, PortIndex) const override;

    QWidget* embeddedWidget() override;

    QJsonObject save() const override;
    void load(QJsonObject const &p) override;

    // non-framework functionality
    virtual bool init() { return true; }

  protected:

    /**
     * Port caption that should be used if no other caption or information is available
     */
    const QString unknownPortCaption = "<UNKNOWN>";
};
