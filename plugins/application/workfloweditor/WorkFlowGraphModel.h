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

#include "QtNodes/internal/AbstractGraphModel.hpp"
#include "QtNodes/internal/ConnectionGraphicsObject.hpp"
#include "QtNodes/BasicGraphicsScene"
#include "QtNodes/internal/NodeGraphicsObject.hpp"
#include "QtNodes/internal/NodeDelegateModelRegistry.hpp"
#include "QtNodes/internal/Serializable.hpp"
#include "QtNodes/BasicGraphicsScene"

#include "QtNodes/internal/Export.hpp"

#include <QJsonObject>
#include <QJsonArray>

#include <QUndoCommand>

#include <memory>
#include <qpoint.h>

// namespace QtNodes {
using QtNodes::PortType;
using QtNodes::PortRole;
using QtNodes::PortIndex;
using QtNodes::NodeId;
using QtNodes::NodeRole;
using QtNodes::NodeFlag;
using QtNodes::NodeFlags;
using QtNodes::NodeData;
using QtNodes::NodeDataType;
using QtNodes::NodeDelegateModel;
using QtNodes::NodeDelegateModelRegistry;
using QtNodes::ConnectionId;
using QtNodes::ConnectionPolicy;
using QtNodes::InvalidNodeId;
using QtNodes::BasicGraphicsScene;


namespace QtNodes
{

    inline
    bool
    operator<(QtNodes::NodeDataType const & d1,
              QtNodes::NodeDataType const & d2)
    {
      return d1.id < d2.id;
    }

}
using QtNodes::AbstractGraphModel;

class NewNodeCommand : public QUndoCommand
{
public:
    NewNodeCommand(BasicGraphicsScene *scene, NodeId nodeId, QPointF const &scenePos)
      : _scene(scene)
      , _nodeId(nodeId)
      , _sceneJson(QJsonObject()) {
        _scene->graphModel().setNodeData(_nodeId, NodeRole::Position, scenePos);
    }

    void undo() override {
      QJsonArray nodesJsonArray;
      nodesJsonArray.append(_scene->graphModel().saveNode(_nodeId));
      _sceneJson["nodes"] = nodesJsonArray;

      _scene->graphModel().deleteNode(_nodeId);
    }

    void redo() override {
      if (_sceneJson.empty() || _sceneJson["nodes"].toArray().empty()) {
        return;
      }

      AbstractGraphModel &graphModel = _scene->graphModel();

      QJsonArray const &nodesJsonArray = _sceneJson["nodes"].toArray();

      for (QJsonValue node : nodesJsonArray) {
        QJsonObject obj = node.toObject();

        graphModel.loadNode(obj);

        auto id = obj["id"].toInt();
        _scene->nodeGraphicsObject(id)->setZValue(1.0);
        _scene->nodeGraphicsObject(id)->setSelected(true);
      }
    }

private:
    BasicGraphicsScene *_scene;
    NodeId _nodeId;
    QJsonObject _sceneJson;
};



class NewSceneCommand : public QUndoCommand
{
public:
    NewSceneCommand(BasicGraphicsScene *scene)
        : _scene(scene)
        , _sceneJson(QJsonObject())
    {
        //
    }

    void addNode(NodeId const nodeId) {
        QJsonArray nodesJsonArray = _sceneJson["nodes"].toArray();
        nodesJsonArray.append(_scene->graphModel().saveNode(nodeId));
        _sceneJson["nodes"] = nodesJsonArray;
    }

    void addConnection(ConnectionId const connectionId) {
        QJsonArray connJsonArray = _sceneJson["connections"].toArray();

        QJsonObject connJson;

        connJson["out_id"]    = static_cast<qint64>(connectionId.outNodeId);
        connJson["out_index"] = static_cast<qint64>(connectionId.outPortIndex);
        connJson["in_id"]     = static_cast<qint64>(connectionId.inNodeId);
        connJson["in_index"]  = static_cast<qint64>(connectionId.inPortIndex);

        connJsonArray.append(connJson);

        _sceneJson["connections"] = connJsonArray;
    }

    void undo() override
    {
      QJsonArray connectionJsonArray = _sceneJson["connections"].toArray();

      for (QJsonValueRef connection : connectionJsonArray) {
        QJsonObject connJson = connection.toObject();

        ConnectionId connId{
                    static_cast<NodeId>(connJson["out_id"].toInt(InvalidNodeId)),
                    static_cast<PortIndex>(connJson["out_index"].toInt(QtNodes::InvalidPortIndex)),
                    static_cast<NodeId>(connJson["in_id"].toInt(InvalidNodeId)),
                    static_cast<PortIndex>(connJson["in_index"].toInt(QtNodes::InvalidPortIndex))};

        // delete the connection
        _scene->graphModel().deleteConnection(connId);
      }

      QJsonArray const &nodesJsonArray = _sceneJson["nodes"].toArray();

      for (QJsonValue node : nodesJsonArray) {
        QJsonObject obj = node.toObject();

        auto id = obj["id"].toInt();
        _scene->graphModel().deleteNode(id);
      }
    }

    void redo() override
    {
      if (_sceneJson.empty()) {
        return;
      }

      AbstractGraphModel &graphModel = _scene->graphModel();

      QJsonArray const &nodesJsonArray = _sceneJson["nodes"].toArray();

      for (QJsonValue node : nodesJsonArray) {
        QJsonObject obj = node.toObject();

        graphModel.loadNode(obj);

        auto id = obj["id"].toInt();
        _scene->nodeGraphicsObject(id)->setZValue(1.0);
        _scene->nodeGraphicsObject(id)->setSelected(true);
      }

      QJsonArray connectionJsonArray = _sceneJson["connections"].toArray();

      for (QJsonValueRef connection : connectionJsonArray) {
        QJsonObject connJson = connection.toObject();

        ConnectionId connId{
                    static_cast<NodeId>(connJson["out_id"].toInt(InvalidNodeId)),
                    static_cast<PortIndex>(connJson["out_index"].toInt(QtNodes::InvalidPortIndex)),
                    static_cast<NodeId>(connJson["in_id"].toInt(InvalidNodeId)),
                    static_cast<PortIndex>(connJson["in_index"].toInt(QtNodes::InvalidPortIndex))};

        // Restore the connection
        graphModel.addConnection(connId);
        _scene->connectionGraphicsObject(connId)->setSelected(true);
      }
    }

private:
    BasicGraphicsScene *_scene;
    QJsonObject _sceneJson;
};

class MoveNodesCommand : public QUndoCommand
{
public:
    MoveNodesCommand(BasicGraphicsScene *scene)
        : _scene(scene)
    {
        posmap.clear();
        setObsolete(true);
    }

    void addNodePos(NodeId const nodeId, QPointF const new_pos) {
        auto oldPos = _scene->graphModel().nodeData(nodeId, NodeRole::Position).value<QPointF>();
        QPointF diff = oldPos - new_pos;
        posmap[nodeId] = diff;
        if (diff != QPointF{0.0, 0.0}) setObsolete(false);
    }

    void undo() override
    {
      for (auto it = posmap.keyValueBegin(); it != posmap.keyValueEnd(); ++it) {
        NodeId  nodeId = it->first;
        QPointF diff   = it->second;

        auto oldPos = _scene->graphModel().nodeData(nodeId, NodeRole::Position).value<QPointF>();

        oldPos += diff;

        _scene->graphModel().setNodeData(nodeId, NodeRole::Position, oldPos);
      }
    }

    void redo() override
    {
      for (auto it = posmap.keyValueBegin(); it != posmap.keyValueEnd(); ++it) {
        NodeId  nodeId = it->first;
        QPointF diff   = it->second;

        auto oldPos = _scene->graphModel().nodeData(nodeId, NodeRole::Position).value<QPointF>();

        oldPos -= diff;

        _scene->graphModel().setNodeData(nodeId, NodeRole::Position, oldPos);
      }
    }

private:
    BasicGraphicsScene *_scene;
    QMap<NodeId, QPointF> posmap;
};

class NODE_EDITOR_PUBLIC WorkFlowGraphModel : public QtNodes::AbstractGraphModel, public QtNodes::Serializable
{
    Q_OBJECT

public:
    struct NodeGeometryData
    {
        QSize size;
        QPointF pos;
    };

public:
    WorkFlowGraphModel(std::shared_ptr<NodeDelegateModelRegistry> registry);

    std::shared_ptr<NodeDelegateModelRegistry> dataModelRegistry() { return _registry; }

public:
    std::unordered_set<NodeId> allNodeIds() const override;

    std::unordered_set<ConnectionId> allConnectionIds(NodeId const nodeId) const override;
    std::unordered_set<ConnectionId> allConnectionIds() const /*override*/;

    std::unordered_set<ConnectionId> connections(NodeId nodeId,
                                                 PortType portType) const /*override*/;

    std::unordered_set<ConnectionId> connections(NodeId nodeId,
                                                 PortType portType,
                                                 PortIndex portIndex) const override;

    bool connectionExists(ConnectionId const connectionId) const override;

    NodeId addNode(QString const nodeType) override;

    bool connectionPossible(ConnectionId const connectionId) const override;

    void addConnection(ConnectionId const connectionId) override;

    bool nodeExists(NodeId const nodeId) const override;

    QVariant nodeData(NodeId nodeId, NodeRole role) const override;

    NodeFlags nodeFlags(NodeId nodeId) const override;

    bool setNodeData(NodeId nodeId, NodeRole role, QVariant value) override;

    QVariant portData(NodeId nodeId,
                      PortType portType,
                      PortIndex portIndex,
                      PortRole role) const override;

    bool setPortData(NodeId nodeId,
                     PortType portType,
                     PortIndex portIndex,
                     QVariant const &value,
                     PortRole role = PortRole::Data) override;

    bool deleteConnection(ConnectionId const connectionId) override;

    bool deleteNode(NodeId const nodeId) override;

    QJsonObject saveNode(NodeId const) const override;
    QJsonObject saveNode2(NodeId const) const;

    QJsonObject save() const override;

    void loadNode(QJsonObject const &nodeJson, NodeId restoredNodeId);
    void loadNode(QJsonObject const &nodeJson) override;

    void load(QJsonObject const &json) override;

    void merge(QJsonObject const &mergescene, const QPointF& offset);

    void setVariables(const QJsonArray &variables);
    void initializeDefaultVariables();
    const QJsonArray& getVariables() const;

    /**
   * Fetches the NodeDelegateModel for the given `nodeId` and tries to cast the
   * stored pointer to the given type
   */
    template<typename NodeDelegateModelType>
    NodeDelegateModelType *delegateModel(NodeId const nodeId)
    {
        auto it = _models.find(nodeId);
        if (it == _models.end())
            return nullptr;

        auto model = dynamic_cast<NodeDelegateModelType *>(it->second.get());

        return model;
    }

    void setScene(QtNodes::BasicGraphicsScene* scene) {
      this->scene = scene;
    }

    using SharedNodeData = std::shared_ptr<NodeData>;

    // a function taking in NodeData and returning NodeData
    using TypeConverter = std::function<SharedNodeData(SharedNodeData)>;

    // data-type-in, data-type-out
    using TypeConverterId = std::pair<NodeDataType, NodeDataType>;

    using RegisteredTypeConvertersMap = std::map<TypeConverterId, TypeConverter>;

    void registerTypeConverter(TypeConverterId const & id,
                               TypeConverter typeConverter) {
      _registeredTypeConverters.insert({id, std::move(typeConverter)});
    }

    TypeConverter
    getTypeConverter(NodeDataType const & d1,
                     NodeDataType const & d2) const
    {
      TypeConverterId converterId = std::make_pair(d1, d2);

      auto it = _registeredTypeConverters.find(converterId);

      if (it != _registeredTypeConverters.end())
      {
        return it->second;
      }

      return TypeConverter{};
    }


Q_SIGNALS:
    void inPortDataWasSet(NodeId const, PortType const, PortIndex const);

private:
    NodeId newNodeId() override { return _nextNodeId++; }

    void connectNode(std::unique_ptr<NodeDelegateModel>& model, NodeId newId);

    void sendConnectionCreation(ConnectionId const connectionId);

    void sendConnectionDeletion(ConnectionId const connectionId);

private Q_SLOTS:
    /**
   * Fuction is called in three cases:
   *
   * - By underlying NodeDelegateModel when a node has new data to propagate.
   *   @see WorkFlowGraphModel::addNode
   * - When a new connection is created.
   *   @see WorkFlowGraphModel::addConnection
   * - When a node restored from JSON an needs to send data downstream.
   *   @see WorkFlowGraphModel::loadNode
   */
    void onOutPortDataUpdated(NodeId const nodeId, PortIndex const portIndex);

    /// Function is called after detaching a connection.
    void propagateEmptyDataTo(NodeId const nodeId, PortIndex const portIndex);

private:
    std::shared_ptr<NodeDelegateModelRegistry> _registry;

    NodeId _nextNodeId;

    std::unordered_map<NodeId, std::unique_ptr<NodeDelegateModel>> _models;

    std::unordered_set<ConnectionId> _connectivity;

    mutable std::unordered_map<NodeId, NodeGeometryData> _nodeGeometryData;

    QtNodes::BasicGraphicsScene *scene;

    RegisteredTypeConvertersMap _registeredTypeConverters;

    QJsonArray variables;

};

// } // namespace QtNodes
