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

#include <utility>
#include <QDebug>
#include <framework/pluginframework/pluginmanager.h>
#include "../../widgets/executionprofilechooser.h"
#include "../../widgets/inserttooldialog.h"
#include "nodes/data/workflowdatatypes.h"
#include "plugins/application/workfloweditor/config.h"
#include "plugins/application/workfloweditor/nodes/models/workflownode.h"

#include "toolnode.h"


ToolNode::ToolNode() : WorkflowNode(),
    executionProfile(ExecutionProfile::DEFAULT),
    executionProfileChooser(new ExecutionProfileChooser()) {

  tool = std::make_unique<Tool>();

  // might be change in init()
  setNodeStyle(EditorConfig::TOOL_STYLE);
  modelName = "ToolNode";

  executionProfileChooser->addSelectableProfile(ExecutionProfile::DEFAULT);
  executionProfileChooser->addSelectableProfile(ExecutionProfile::SKIP);
  executionProfileChooser->addSelectableProfile(ExecutionProfile::DETACHED);

  connect(executionProfileChooser, &ExecutionProfileChooser::executionProfileSelected, this,
          [&, this](ExecutionProfile profile) {
            executionProfile = profile;
            setNodeStyleBasedOnProfile();
          });
}

QString ToolNode::caption() const {
  QString caption = "Unnamed tool";
  if (!tool->getName().isEmpty()) {
    caption = tool->getName() + " " + tool->getVersion();
  }
  return caption;
}

QString ToolNode::name() const {
  return modelName;
}

unsigned int ToolNode::nPorts(PortType portType) const {
  switch (portType) {
    case PortType::In:
      return tool->getInputParameterCount();

    case PortType::Out:
      return tool->getOutputParameterCount();

    case PortType::None:
    default:
      return 1;
  }
}

NodeDataType ToolNode::dataType(PortType portType,
                                PortIndex portIndex) const {
  QString typeName;
  try {
    if (portType == PortType::In) {
      typeName = tool->getInputParameter(portIndex)->getType();
      return typeNameToType(typeName);
    }
    if (portType == PortType::Out) {
      typeName = tool->getOutputParameter(portIndex)->getType();
      return typeNameToType(typeName);
    }
  } catch(std::exception const& exception) {
    qDebug() << "Error fetching parameter info: " << exception.what();
  }

  return {};
}

void ToolNode::setTool(std::unique_ptr<Tool> t) {
  this->tool = std::move(t);
}

QString ToolNode::portCaption(PortType portType, PortIndex portIndex) const {
  QString result;
  QString param_type;
  QString param_name;

  try {
    if (portType == PortType::In) {
      if (tool->getInputParameter(portIndex)->getType() == "dependency") {
        return "Dependencies";
      }
      if (tool->getInputParameter(portIndex)->getType() == "pipe") {
        return "stdin";
      }
      if (tool->getInputParameter(portIndex)->getType() == "env") {
        return "env";
      }
      param_name = tool->getInputParameter(portIndex)->isRequired() ? "*" : "";
      param_name += tool->getInputParameter(portIndex)->getLongName();
      param_type = tool->getInputParameter(portIndex)->getType();
      result += param_type + ": " + param_name;
    }
    if (portType == PortType::Out) {
      if (tool->getOutputParameter(portIndex)->getType() == "dependency") {
        return "Dependents";
      }
      if (tool->getOutputParameter(portIndex)->getType() == "pipe") {
        return "stdout";
      }
      if (tool->getOutputParameter(portIndex)->getType() == "env") {
        return "env";
      }
      param_name = tool->getOutputParameter(portIndex)->getLongName();
      param_name += tool->getOutputParameter(portIndex)->isRequired() ? "*" : "";
      param_type = tool->getOutputParameter(portIndex)->getType();
      result = param_name + " :" + param_type;
    }
  } catch (std::exception const& exception) {
    qWarning() << "Error fetching parameter info: " << exception.what();
  }
  return result;
}

NodeDataType ToolNode::typeNameToType(const QString& typeName) {
  if (typeName == "dependency") {
    return DataTypes::DEPENDENCY;
  }
  if (typeName == "filein" || typeName == "fileout") {
    return DataTypes::STRING;
  }
  if (typeName == "bool" || typeName == "flag") {
    return DataTypes::BOOLEAN;
  }
  if (typeName == "real" || typeName == "float") {
    return DataTypes::FLOAT;
  }
  if (typeName == "integer" || typeName == "long") {
    return DataTypes::INTEGER;
  }
  if (typeName == "pipe") {
    return DataTypes::PIPE;
  }
  if (typeName == "env") {
    return DataTypes::ENV;
  }
  return DataTypes::STRING;
}

QJsonObject ToolNode::save() const {
  QJsonObject jsonObject = WorkflowNode::save();
  jsonObject["executionProfile"] = ExecutionProfiles::name(executionProfile);
  jsonObject["tool"] = tool->toJson();

  return jsonObject;
}

#include <QGraphicsProxyWidget>

// void ToolNode::addCornerWidget(QJsonObject const& p) {
//   if (auto w = cornerWidget()) {
//
//     auto _cornerProxyWidget = new QGraphicsProxyWidget(ngo);
//
//     _cornerProxyWidget->setWidget(w);
//
//     _cornerProxyWidget->setPreferredWidth(5);
//
//     geometry.recomputeSize(nodeId);
//
//     if (w->sizePolicy().verticalPolicy() & QSizePolicy::ExpandFlag)
//     {
//       // If the widget wants to use as much vertical space as possible, set it to have the geom's equivalentWidgetHeight.
//       // _cornerProxyWidget->setMinimumHeight(geometry.equivalentCornerWidgetHeight());
//     }
//
//     QPointF pos;
//     pos.setX(geometry.widgetPosition(nodeId).x()+geometry.captionRect(nodeId).width()+5);
//     pos.setY(geometry.widgetPosition(nodeId).y()-25);
//
//     _cornerProxyWidget->setPos(pos);
//
//     update();
//
//     _cornerProxyWidget->setOpacity(1.0);
//     // _cornerProxyWidget->setFlag(QGraphicsItem::ItemIgnoresParentOpacity);
//   }
// }

void ToolNode::load(QJsonObject const& p) {
  if (p.contains("executionProfile")) {
    executionProfile = ExecutionProfiles::fromString(p["executionProfile"].toString());
  } else {
    executionProfile = ExecutionProfiles::ExecutionProfile::DEFAULT;
  }
  executionProfileChooser->setSelectedProfile(executionProfile);
  QJsonObject toolObject = p["tool"].toObject();
  tool = std::make_unique<Tool>(toolObject);
}

bool ToolNode::init() {
  auto tooldialog = new InsertToolDialog(LibFramework::PluginManager::getInstance(), nullptr/*this*/);
  QMap<QString, QVariant> tooldatamap = tooldialog->getSelectedToolInformation();

  if (!tooldatamap.isEmpty()) {
    // initialize tool configuration
    std::unique_ptr<Tool> toolFromXml = Tool::create(tooldatamap); // TODO Merge Tool and ToolDescription (toolchooser), then request the Tool object directly to avoid reparsing it here

    // create the actual node
    // create the node model (ToolNode (default) or EnvNode)
    if (toolFromXml->isEnv()) {
      modelName =  "EnvNode";
      setNodeStyle(EditorConfig::ENV_STYLE);
    }

    // inject the tool description into the node model
    setTool(std::move(toolFromXml)); // this works because EnvNode is a subclass of ToolNode!
    return true;
  }
  return false;
}

ConnectionPolicy ToolNode::portConnectionPolicy(PortType portType, PortIndex portIndex) const {
  if (portType == PortType::Out && portIndex == 1) {
    return ConnectionPolicy::One;
  } else if (portType == PortType::In && portIndex == 0) {
    return ConnectionPolicy::Many;
  } else {
    NodeDataType type = dataType(portType, portIndex);
    if (type.id == DataTypes::PIPE.id) {
      return ConnectionPolicy::One;
    }
    return NodeDelegateModel::portConnectionPolicy(portType, portIndex);
  }
}

QWidget* ToolNode::cornerWidget() {
  return (tool->isEnv()) ? nullptr : executionProfileChooser;
}

void ToolNode::setExecutionProfile(ExecutionProfile profile) {
  executionProfile = profile;
  executionProfileChooser->setSelectedProfile(executionProfile);
  setNodeStyleBasedOnProfile();
}

ExecutionProfile ToolNode::getExecutionProfile() const {
  return executionProfile;
}

void ToolNode::setNodeStyleBasedOnProfile() {
  if (executionProfile == ExecutionProfiles::ExecutionProfile::DEFAULT) {
    // this applies the "default" style based on the node's subtype
    setNodeStyle(EditorConfig::TOOL_STYLE);
  } else if (executionProfile == ExecutionProfiles::ExecutionProfile::SKIP) {
    QtNodes::NodeStyle style = nodeStyle();
    QColor skipBrushColor = style.GradientColor0;
    skipBrushColor.setAlpha(0); // make brush/fill completely invisible
    style.GradientColor0 = skipBrushColor;
    style.GradientColor1 = skipBrushColor;
    style.GradientColor2 = skipBrushColor;
    style.GradientColor3 = skipBrushColor;
    style.FontColor = QColor(100, 100, 100);
    style.FontColorFaded = QColor(10, 10, 10);
    setNodeStyle(style);
  } else if (executionProfile == ExecutionProfiles::ExecutionProfile::DETACHED) {
    QtNodes::NodeStyle style = nodeStyle();
    QColor skipBrushColor = style.GradientColor0;
    skipBrushColor.setAlpha(67);
    style.GradientColor0 = skipBrushColor;
    style.GradientColor1 = skipBrushColor;
    style.GradientColor2 = skipBrushColor;
    style.GradientColor3 = skipBrushColor;
    style.FontColor = QColor(100, 100, 100);
    style.FontColorFaded = QColor(10, 10, 10);
    setNodeStyle(style);
  }
}
