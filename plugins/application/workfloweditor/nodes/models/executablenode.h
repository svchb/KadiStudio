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

// #include <QtNodes/internal/AbstractNodeGeometry.hpp>
// #include <QtNodes/BasicGraphicsScene>
// #include <QtNodes/NodeDelegateModel>
// #include <QtNodes/DefaultNodePainter>
// #include <QtNodes/NodeStyle>
// #include <QtNodes/internal/NodeGraphicsObject.hpp>
// #include <QtNodes/NodeData>
// #include <QComboBox>
// #include <QToolBar>
// #include <QJsonDocument>
// #include <QDebug>

// #include "workflownode.h"

// using QtNodes::AbstractGraphModel;
// using QtNodes::AbstractNodeGeometry;
// using QtNodes::NodeId;
// using QtNodes::NodeData;
// using QtNodes::NodeDataType;
// using QtNodes::NodeDelegateModel;
// using QtNodes::PortType;
// using QtNodes::PortIndex;
// using QtNodes::DefaultNodePainter;
// using QtNodes::NodeStyle;
// using QtNodes::NodeRole;
// using QtNodes::NodeGraphicsObject;


/**
 * @brief      Superclass for all node models for executable nodes, such
 *             as ToolNode and EnvNode. Adds execution profile and a UI
 *             element to let the user switch the execution profile.
 * @ingroup    models
 */
// class ExecutableNodePainterDelegate : public QtNodes::DefaultNodePainter {
//   // this will be called additionally to normal node painting
//   void paint(QPainter *painter, NodeGraphicsObject &ngo) const override {
//     // QtNodes::DefaultNodePainter::paint(painter, ngo);
//
//     // AbstractGraphModel &model = ngo.graphModel();
//     // NodeId const nodeId = ngo.nodeId();
//     // AbstractNodeGeometry &geometry = ngo.nodeScene()->nodeGeometry();
//     //
//     // geometry.recomputeSize(nodeId);
//     //
//     // QJsonDocument json = QJsonDocument::fromVariant(model.nodeData(nodeId, NodeRole::Style));
//     // NodeStyle nodeStyle(json.object());
//     //
//     // float diam = nodeStyle.ConnectionPointDiameter;
//     //
//     // QRectF geom = ngo.boundingRect();
//     //
//     // QRectF boundary(-diam, -diam, 2.0 * diam + geom.width(), 2.0 * diam + geom.height());
//     // painter->setPen(QColor("black"));
//     // painter->drawText(QPointF(boundary.top(), -15), "Node will be skipped");
//   }
// };

// class ExecutableNode : public WorkflowNode {
//   Q_OBJECT
//
//   public:
//     ExecutableNode();
//     ~ExecutableNode() override = default;
//
//     // QString caption() const override = 0;
//     // QString name() const override = 0;
//     // unsigned int nPorts(PortType portType) const override = 0;
//     // NodeDataType dataType(PortType portType, PortIndex portIndex) const override = 0;
//     // QString portCaption(PortType portType, PortIndex portIndex) const override;
//     // ConnectionPolicy portConnectionPolicy(PortType portType, PortIndex portIndex) const override;
//     // QWidget* cornerWidget() override;
//     // NodePainterDelegate* painterDelegate() const override;
//
//   private:
//
// };
