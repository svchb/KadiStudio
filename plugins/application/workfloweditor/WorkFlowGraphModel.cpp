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

#include "WorkFlowGraphModel.h"
#include "QtNodes/internal/ConnectionIdUtils.hpp"
#include "QtNodes/internal/StyleCollection.hpp"
#include "nodes/models/workflownode.h"
#include "plugins/application/workfloweditor/config.h"

#include <QJsonArray>
#include <QUndoCommand>

#include <algorithm>
#include <stdexcept>

using QtNodes::StyleCollection;

// namespace QtNodes {

WorkFlowGraphModel::WorkFlowGraphModel(std::shared_ptr<NodeDelegateModelRegistry> registry)
    : _registry(std::move(registry))
    , _nextNodeId{0}
{}

std::unordered_set<NodeId> WorkFlowGraphModel::allNodeIds() const
{
    std::unordered_set<NodeId> nodeIds;
    for_each(_models.begin(), _models.end(), [&nodeIds](auto const &p) { nodeIds.insert(p.first); });

    return nodeIds;
}

std::unordered_set<ConnectionId> WorkFlowGraphModel::allConnectionIds(NodeId const nodeId) const
{
    std::unordered_set<ConnectionId> result;

    std::copy_if(_connectivity.begin(),
                 _connectivity.end(),
                 std::inserter(result, std::end(result)),
                 [&nodeId](ConnectionId const &cid) {
                     return cid.inNodeId == nodeId || cid.outNodeId == nodeId;
                 });

    return result;
}

std::unordered_set<ConnectionId> WorkFlowGraphModel::allConnectionIds() const
{
    return _connectivity;
}

std::unordered_set<ConnectionId> WorkFlowGraphModel::connections(NodeId nodeId,
                                                                 PortType portType) const
{
    std::unordered_set<ConnectionId> result;

    std::copy_if(_connectivity.begin(),
                 _connectivity.end(),
                 std::inserter(result, std::end(result)),
                 [&portType, &nodeId](ConnectionId const &cid) {
                     return (getNodeId(portType, cid) == nodeId);
                 });

    return result;
}

std::unordered_set<ConnectionId> WorkFlowGraphModel::connections(NodeId nodeId,
                                                                 PortType portType,
                                                                 PortIndex portIndex) const
{
    std::unordered_set<ConnectionId> result;

    std::copy_if(_connectivity.begin(),
                 _connectivity.end(),
                 std::inserter(result, std::end(result)),
                 [&portType, &portIndex, &nodeId](ConnectionId const &cid) {
                     return (getNodeId(portType, cid) == nodeId
                             && getPortIndex(portType, cid) == portIndex);
                 });

    return result;
}

bool WorkFlowGraphModel::connectionExists(ConnectionId const connectionId) const
{
    return (_connectivity.find(connectionId) != _connectivity.end());
}

void WorkFlowGraphModel::connectNode(std::unique_ptr<NodeDelegateModel>& model, NodeId newId) {
    connect(model.get(),
            &NodeDelegateModel::dataUpdated,
            [newId, this](PortIndex const portIndex) {
                onOutPortDataUpdated(newId, portIndex);
            });

    connect(model.get(),
            &NodeDelegateModel::portsAboutToBeDeleted,
            this,
            [newId, this](PortType const portType, PortIndex const first, PortIndex const last) {
                portsAboutToBeDeleted(newId, portType, first, last);
            });

    connect(model.get(),
            &NodeDelegateModel::portsDeleted,
            this,
            &WorkFlowGraphModel::portsDeleted);

    connect(model.get(),
            &NodeDelegateModel::portsAboutToBeInserted,
            this,
            [newId, this](PortType const portType, PortIndex const first, PortIndex const last) {
                portsAboutToBeInserted(newId, portType, first, last);
            });

    connect(model.get(),
            &NodeDelegateModel::portsInserted,
            this,
            &WorkFlowGraphModel::portsInserted);

    connect(model.get(),
            &NodeDelegateModel::embeddedWidgetSizeUpdated,
            [newId, this]() {
                scene->onNodeUpdated(newId);
            });

}

NodeId WorkFlowGraphModel::addNode(QString const nodeType)
{
    std::unique_ptr<NodeDelegateModel> model = _registry->create(nodeType);

    if (model) {
        // give the node model opportunity to initialize its configuration
        WorkflowNode *workflownode = dynamic_cast<WorkflowNode*>(model.get());
        if (workflownode && !workflownode->init()) {
          delete model.release();
          return InvalidNodeId;
        }

        NodeId newId = newNodeId();

        connectNode(model, newId);

        _models[newId] = std::move(model);

        Q_EMIT nodeCreated(newId);

        return newId;
    }

    return InvalidNodeId;
}

bool WorkFlowGraphModel::connectionPossible(ConnectionId const connectionId) const
{
    auto getDataType = [&](PortType const portType) {
        return portData(getNodeId(portType, connectionId),
                        portType,
                        getPortIndex(portType, connectionId),
                        PortRole::DataType)
            .value<NodeDataType>();
    };

    auto portVacant = [&](PortType const portType) {
        NodeId const nodeId = getNodeId(portType, connectionId);
        PortIndex const portIndex = getPortIndex(portType, connectionId);
        auto const connected = connections(nodeId, portType, portIndex);

        auto policy = portData(nodeId, portType, portIndex, PortRole::ConnectionPolicyRole)
                          .value<ConnectionPolicy>();

        return connected.empty() || (policy == ConnectionPolicy::Many);
    };

    bool compatible = getDataType(PortType::Out).id == getDataType(PortType::In).id
                   && portVacant(PortType::Out) && portVacant(PortType::In);

    if (not compatible) {
      compatible = (getTypeConverter(getDataType(PortType::Out), getDataType(PortType::In)) != nullptr);
    }

    return compatible;
}

void WorkFlowGraphModel::addConnection(ConnectionId const connectionId)
{
    _connectivity.insert(connectionId);

    sendConnectionCreation(connectionId);

    QVariant const portDataToPropagate = portData(connectionId.outNodeId,
                                                  PortType::Out,
                                                  connectionId.outPortIndex,
                                                  PortRole::Data);

    setPortData(connectionId.inNodeId,
                PortType::In,
                connectionId.inPortIndex,
                portDataToPropagate,
                PortRole::Data);
}

void WorkFlowGraphModel::sendConnectionCreation(ConnectionId const connectionId)
{
    Q_EMIT connectionCreated(connectionId);

    auto iti = _models.find(connectionId.inNodeId);
    auto ito = _models.find(connectionId.outNodeId);
    if (iti != _models.end() && ito != _models.end()) {
        auto &modeli = iti->second;
        auto &modelo = ito->second;
        modeli->inputConnectionCreated(connectionId);
        modelo->outputConnectionCreated(connectionId);
    }
}

void WorkFlowGraphModel::sendConnectionDeletion(ConnectionId const connectionId)
{
    Q_EMIT connectionDeleted(connectionId);

    auto iti = _models.find(connectionId.inNodeId);
    auto ito = _models.find(connectionId.outNodeId);
    if (iti != _models.end() && ito != _models.end()) {
        auto &modeli = iti->second;
        auto &modelo = ito->second;
        modeli->inputConnectionDeleted(connectionId);
        modelo->outputConnectionDeleted(connectionId);
    }
}

bool WorkFlowGraphModel::nodeExists(NodeId const nodeId) const
{
    return (_models.find(nodeId) != _models.end());
}

QVariant WorkFlowGraphModel::nodeData(NodeId nodeId, NodeRole role) const
{
    QVariant result;

    auto it = _models.find(nodeId);
    if (it == _models.end())
        return result;

    auto &model = it->second;

    switch (role) {
    case NodeRole::Type:
        result = model->name();
        break;

    case NodeRole::Position:
        result = _nodeGeometryData[nodeId].pos;
        break;

    case NodeRole::Size:
        result = _nodeGeometryData[nodeId].size;
        break;

    case NodeRole::CaptionVisible:
        result = model->captionVisible();
        break;

    case NodeRole::Caption:
        result = model->caption();
        break;

    case NodeRole::Style: {
        auto style = _models.at(nodeId)->nodeStyle();
        result = style.toJson().toVariantMap();
    } break;

    case NodeRole::InternalData: {
        QJsonObject nodeJson;

        nodeJson["model"] = _models.at(nodeId)->save();

        result = nodeJson.toVariantMap();
        break;
    }

    case NodeRole::InPortCount:
        result = model->nPorts(PortType::In);
        break;

    case NodeRole::OutPortCount:
        result = model->nPorts(PortType::Out);
        break;

    case NodeRole::Widget: {
        auto w = model->embeddedWidget();
        result = QVariant::fromValue(w);
    } break;
    }

    return result;
}

NodeFlags WorkFlowGraphModel::nodeFlags(NodeId nodeId) const
{
    auto it = _models.find(nodeId);

    if (it != _models.end() && it->second->resizable())
        return NodeFlag::Resizable;

    return NodeFlag::NoFlags;
}

bool WorkFlowGraphModel::setNodeData(NodeId nodeId, NodeRole role, QVariant value)
{
    Q_UNUSED(nodeId);
    Q_UNUSED(role);
    Q_UNUSED(value);

    bool result = false;

    switch (role) {
    case NodeRole::Type:
        break;

    case NodeRole::Position: {
        _nodeGeometryData[nodeId].pos = value.value<QPointF>();

        Q_EMIT nodePositionUpdated(nodeId);

        result = true;
    } break;

    case NodeRole::Size: {
        _nodeGeometryData[nodeId].size = value.value<QSize>();
        result = true;
    } break;

    case NodeRole::CaptionVisible:
        break;

    case NodeRole::Caption:
        break;

    case NodeRole::Style:
        _models.at(nodeId)->setNodeStyle(value.value<QString>());
        break;

    case NodeRole::InternalData:
        break;

    case NodeRole::InPortCount:
        break;

    case NodeRole::OutPortCount:
        break;

    case NodeRole::Widget:
        break;
    }

    return result;
}

QVariant WorkFlowGraphModel::portData(NodeId nodeId,
                                      PortType portType,
                                      PortIndex portIndex,
                                      PortRole role) const
{
    QVariant result;

    auto it = _models.find(nodeId);
    if (it == _models.end())
        return result;

    auto &model = it->second;

    switch (role) {
    case PortRole::Data:
        if (portType == PortType::Out)
            result = QVariant::fromValue(model->outData(portIndex));
        break;

    case PortRole::DataType:
        result = QVariant::fromValue(model->dataType(portType, portIndex));
        break;

    case PortRole::ConnectionPolicyRole:
        result = QVariant::fromValue(model->portConnectionPolicy(portType, portIndex));
        break;

    case PortRole::CaptionVisible:
        result = model->portCaptionVisible(portType, portIndex);
        break;

    case PortRole::Caption:
        result = model->portCaption(portType, portIndex);
        break;
    }

    return result;
}

bool WorkFlowGraphModel::setPortData(
    NodeId nodeId, PortType portType, PortIndex portIndex, QVariant const &value, PortRole role)
{
    Q_UNUSED(nodeId);

    QVariant result;

    auto it = _models.find(nodeId);
    if (it == _models.end())
        return false;

    auto &model = it->second;

    switch (role) {
    case PortRole::Data:
        if (portType == PortType::In) {
            model->setInData(value.value<std::shared_ptr<NodeData>>(), portIndex);

            // Triggers repainting on the scene.
            Q_EMIT inPortDataWasSet(nodeId, portType, portIndex);
        }
        break;

    default:
        break;
    }

    return false;
}

bool WorkFlowGraphModel::deleteConnection(ConnectionId const connectionId)
{
    bool disconnected = false;

    auto it = _connectivity.find(connectionId);

    if (it != _connectivity.end()) {
        disconnected = true;

        _connectivity.erase(it);
    }

    if (disconnected) {
        sendConnectionDeletion(connectionId);

        propagateEmptyDataTo(getNodeId(PortType::In, connectionId),
                             getPortIndex(PortType::In, connectionId));
    }

    return disconnected;
}

bool WorkFlowGraphModel::deleteNode(NodeId const nodeId)
{
    // Delete connections to this node first.
    auto connectionIds = allConnectionIds(nodeId);
    for (auto &cId : connectionIds) {
        deleteConnection(cId);
    }

    _nodeGeometryData.erase(nodeId);
    _models.erase(nodeId);

    Q_EMIT nodeDeleted(nodeId);

    return true;
}

QJsonObject WorkFlowGraphModel::saveNode(NodeId const nodeId) const
{
    QJsonObject nodeJson;

    nodeJson["id"] = static_cast<qint64>(nodeId);

    nodeJson["model"] = _models.at(nodeId)->save();

    {
        QPointF const pos = nodeData(nodeId, NodeRole::Position).value<QPointF>();

        QJsonObject posJson;
        posJson["x"] = pos.x();
        posJson["y"] = pos.y();
        nodeJson["position"] = posJson;
    }

    return nodeJson;
}

QJsonObject WorkFlowGraphModel::saveNode2(NodeId const nodeId) const
{
    QJsonObject nodeJson;

    nodeJson["id"] = QString::number(static_cast<qint64>(nodeId));

    nodeJson["model"] = _models.at(nodeId)->save();

    {
        QPointF const pos = nodeData(nodeId, NodeRole::Position).value<QPointF>();

        QJsonObject posJson;
        posJson["x"] = pos.x();
        posJson["y"] = pos.y();
        nodeJson["position"] = posJson;
    }

    return nodeJson;
}

QJsonObject WorkFlowGraphModel::save() const
{
    QJsonObject sceneJson;

    QJsonArray nodesJsonArray;
    for (auto const nodeId : allNodeIds()) {
        nodesJsonArray.append(saveNode2(nodeId));
    }
    sceneJson["nodes"] = nodesJsonArray;

    QJsonArray connJsonArray;
    for (auto const &cid : _connectivity) {
        QJsonObject connJson;

        connJson["out_id"]    = QString::number(static_cast<qint64>(cid.outNodeId));
        connJson["out_index"] = static_cast<qint64>(cid.outPortIndex);
        connJson["in_id"]     = QString::number(static_cast<qint64>(cid.inNodeId));
        connJson["in_index"]  = static_cast<qint64>(cid.inPortIndex);

        connJsonArray.append(connJson);
    }
    sceneJson["connections"] = connJsonArray;

    sceneJson["variables"] = variables;

    return sceneJson;
}

void WorkFlowGraphModel::loadNode(QJsonObject const &nodeJson, NodeId restoredNodeId)
{
    _nextNodeId = std::max(_nextNodeId, restoredNodeId + 1);

    QJsonObject const internalDataJson = nodeJson["model"].toObject();

    QString delegateModelName = internalDataJson["name"].toString();

    std::unique_ptr<NodeDelegateModel> model = _registry->create(delegateModelName);

    if (!model) {
        throw std::logic_error(std::string("No registered model with name ")
                               + delegateModelName.toLocal8Bit().data());
    }
    model->load(internalDataJson);

    connectNode(model, restoredNodeId);

    _models[restoredNodeId] = std::move(model);

    QJsonObject posJson = nodeJson["position"].toObject();
    QPointF const pos(posJson["x"].toDouble(), posJson["y"].toDouble());

    setNodeData(restoredNodeId, NodeRole::Position, pos);

    Q_EMIT nodeCreated(restoredNodeId);
}

void WorkFlowGraphModel::loadNode(QJsonObject const &nodeJson)
{
    // Possibility of the id clash when reading it from json and not generating a
    // new value.
    // 1. When restoring a scene from a file.
    // Conflict is not possible because the scene must be cleared by the time of
    // loading.
    // 2. When undoing the deletion command.  Conflict is not possible
    // because all the new ids were created past the removed nodes.
    loadNode(nodeJson, nodeJson["id"].toInt());
}

void WorkFlowGraphModel::load(QJsonObject const &jsonDocument)
{
    QJsonArray nodesJsonArray = jsonDocument["nodes"].toArray();

    // do maping for backward compatibility
    QMap<QString, NodeId> nodeIdmap;
    for (QJsonValueRef nodeJson : nodesJsonArray) {
         nodeIdmap[nodeJson.toObject()["id"].toString()] = newNodeId();
    }

    for (QJsonValueRef nodeJson : nodesJsonArray) {
        QString stringid = nodeJson.toObject()["id"].toString();
        NodeId restoredNodeId = nodeIdmap[stringid];
        loadNode(nodeJson.toObject(), restoredNodeId);
    }

    QJsonArray connectionJsonArray = jsonDocument["connections"].toArray();

    for (QJsonValueRef connection : connectionJsonArray) {
        QJsonObject connJson = connection.toObject();

        QMap<QString, NodeId>::const_iterator outNodeIdmapping = nodeIdmap.constFind(connJson["out_id"].toString());
        NodeId outNodeId = (nodeIdmap.cend() != outNodeIdmapping) ? outNodeIdmapping.value() : QtNodes::InvalidNodeId;
        QMap<QString, NodeId>::const_iterator inNodeIdmapping = nodeIdmap.constFind(connJson["in_id"].toString());
        NodeId inNodeId  = (nodeIdmap.cend() != inNodeIdmapping) ? inNodeIdmapping.value() : QtNodes::InvalidNodeId;
        ConnectionId connId{
                    outNodeId,
                    static_cast<PortIndex>(connJson["out_index"].toInt(QtNodes::InvalidPortIndex)),
                    inNodeId,
                    static_cast<PortIndex>(connJson["in_index"].toInt(QtNodes::InvalidPortIndex))};

        // Restore the connection
        addConnection(connId);
    }

    if (jsonDocument.contains("variables")) {
        variables = jsonDocument["variables"].toArray();
    } else {
        variables = {};
    }
}

void WorkFlowGraphModel::merge(QJsonObject const &mergescene, const QPointF& offset)
{
    scene->clearSelection();

    NewSceneCommand *newscenecommand = new NewSceneCommand(scene);

    QMap<QString, NodeId> oldToNewId; // mapping from origin node id to new id (of the node's copy)

    QJsonArray nodesJsonArray = mergescene["nodes"].toArray();

    // step 1: map nodeId and find the smallest x and y coordinate
    QPointF result = {std::numeric_limits<float>::max(), std::numeric_limits<float>::max()};
    for (QJsonValueRef node : nodesJsonArray) {
      QJsonObject const &nodeJson = node.toObject();

      // create new nodeId for mapping
      QString originNodeId = nodeJson["id"].toString();
      NodeId nodeId = newNodeId();
      oldToNewId[originNodeId] = nodeId;

      // find smalles x and y
      QJsonObject positionJson = nodeJson["position"].toObject();
      QPointF     nodePosition(positionJson["x"].toDouble(),
                               positionJson["y"].toDouble());
      if (nodePosition.x() < result.x()) {
        result.setX(nodePosition.x());
      }
      if (nodePosition.y() < result.y()) {
        result.setY(nodePosition.y());
      }
    }
    result -= offset;

    // step 2: create merged nodes
    for (QJsonValueRef node : nodesJsonArray) {
        QJsonObject const &nodeJson = node.toObject();

        QString originNodeId = nodeJson["id"].toString();
        QString modelName = nodeJson["model"].toObject()["name"].toString();

        std::unique_ptr<NodeDelegateModel> model = _registry->create(modelName);

        if (!model) {
            throw std::logic_error(std::string("No registered model with name ")
                                + modelName.toLocal8Bit().data());
        }
        model->load(nodeJson["model"].toObject());

        NodeId mappedNodeId = oldToNewId[originNodeId];

        connectNode(model, mappedNodeId);

        _models[mappedNodeId] = std::move(model);

        QJsonObject positionJson = nodeJson["position"].toObject();
        QPointF     point(positionJson["x"].toDouble(),
                          positionJson["y"].toDouble());

        QPointF posView = point - result;
        scene->graphModel().setNodeData(mappedNodeId, NodeRole::Position, posView);

        newscenecommand->addNode(mappedNodeId);

        Q_EMIT nodeCreated(mappedNodeId);

        scene->nodeGraphicsObject(mappedNodeId)->setSelected(true);
    }

    // step 3: create connections between merged nodes
    QJsonArray connectionJsonArray = mergescene["connections"].toArray();

    for (QJsonValueRef connection : connectionJsonArray) {
        QJsonObject connJson = connection.toObject();

        QMap<QString, NodeId>::const_iterator outNodeIdmapping = oldToNewId.constFind(connJson["out_id"].toString());
        NodeId outNodeId = (oldToNewId.cend() != outNodeIdmapping) ? outNodeIdmapping.value() : QtNodes::InvalidNodeId;
        QMap<QString, NodeId>::const_iterator inNodeIdmapping = oldToNewId.constFind(connJson["in_id"].toString());
        NodeId inNodeId  = (oldToNewId.cend() != inNodeIdmapping) ? inNodeIdmapping.value() : QtNodes::InvalidNodeId;
        ConnectionId connId{
                    outNodeId,
                    static_cast<PortIndex>(connJson["out_index"].toInt(QtNodes::InvalidPortIndex)),
                    inNodeId,
                    static_cast<PortIndex>(connJson["in_index"].toInt(QtNodes::InvalidPortIndex))};

        // Restore the connection
        addConnection(connId);

        newscenecommand->addConnection(connId);
    }

    // step 4: merge variables
    QJsonArray mergescenevariables = mergescene["variables"].toArray();
    for (int i = 0; i < mergescenevariables.size(); i++) {
        variables.append(mergescenevariables[i]);
    }

    scene->undoStack().push(newscenecommand);
}

void WorkFlowGraphModel::setVariables(const QJsonArray& vars) {
  variables = vars;
}

void WorkFlowGraphModel::initializeDefaultVariables() {
  variables = {
          QJsonObject({
                        {"name", "TMP_FOLDER"},
                        {"value", "tmp"}
                      })
  };
}

const QJsonArray& WorkFlowGraphModel::getVariables() const {
  return variables;
}

void WorkFlowGraphModel::onOutPortDataUpdated(NodeId const nodeId, PortIndex const portIndex)
{
    std::unordered_set<ConnectionId> const &connected = connections(nodeId,
                                                                    PortType::Out,
                                                                    portIndex);

    QVariant const portDataToPropagate = portData(nodeId, PortType::Out, portIndex, PortRole::Data);

    for (auto const &cn : connected) {
        setPortData(cn.inNodeId, PortType::In, cn.inPortIndex, portDataToPropagate, PortRole::Data);
    }

    Q_EMIT nodeUpdated(nodeId);
}

void WorkFlowGraphModel::propagateEmptyDataTo(NodeId const nodeId, PortIndex const portIndex)
{
    QVariant emptyData{};

    setPortData(nodeId, PortType::In, portIndex, emptyData, PortRole::Data);
}

// } // namespace QtNodes
