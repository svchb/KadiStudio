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

#include <QDebug>
#include <QtCore/QJsonArray>

#include "tool.h"

Tool::Tool(const QJsonObject& jsonObject) {
  path = jsonObject["path"].toString();
  name = jsonObject["name"].toString();
  if (name.isEmpty()) {
    name = jsonObject["shortName"].toString(); // for backward compatibility
  }
  version = jsonObject["version"].toString();
  isEnvTool = jsonObject["isEnv"].toBool();

  QJsonArray portsJson = jsonObject["ports"].toArray();
  for (auto port : portsJson) {
    QJsonObject portJson = port.toObject();
    QString direction = portJson["port_direction"].toString();
    if (direction == "in") {
      inputParameters.push_back(std::make_unique<ToolParameter>(portJson));
    } else if (direction == "out") {
      outputParameters.push_back(std::make_unique<ToolParameter>(portJson));
    }
  }
}

const QString& Tool::getName() const {
  return name;
}

void Tool::setName(const QString& name) {
  Tool::name = name;
}

const QString& Tool::getVersion() const {
  return version;
}

void Tool::setVersion(const QString& version) {
  Tool::version = version;
}

const QString& Tool::getDescription() const {
  return description;
}

void Tool::setDescription(const QString& description) {
  Tool::description = description;
}

const QString& Tool::getExample() const {
  return example;
}

void Tool::setExample(const QString& example) {
  Tool::example = example;
}

void Tool::addInputParameter(std::unique_ptr<ToolParameter> parameter) {
  inputParameters.push_back(std::move(parameter));
}

void Tool::addOutputParameter(std::unique_ptr<ToolParameter> parameter) {
  outputParameters.push_back(std::move(parameter));
}

int Tool::getInputParameterCount() const {
  return inputParameters.size();
}

int Tool::getOutputParameterCount() const {
  return outputParameters.size();
}


ToolParameter* Tool::getInputParameter(int index) const {
  return inputParameters.at(index).get();
}

ToolParameter* Tool::getOutputParameter(int index) const {
  return outputParameters.at(index).get();
}

QJsonObject Tool::toJson() {
  QJsonObject toolJsonObj;
  toolJsonObj["path"] = path;
  toolJsonObj["name"] = name;
  toolJsonObj["version"] = version;

  QJsonArray ports;
  for (auto port = inputParameters.begin(); port != inputParameters.end(); ++port) {
    QJsonObject portJsonObj = port->get()->toJson();
    portJsonObj["port_index"] = static_cast<int>(std::distance(inputParameters.begin(), port));
    portJsonObj["port_direction"] = "in";
    ports.push_back(portJsonObj);
  }

  for (auto port = outputParameters.begin(); port != outputParameters.end(); ++port) {
    QJsonObject portJsonObj = port->get()->toJson();
    portJsonObj["port_index"] = static_cast<int>(std::distance(outputParameters.begin(), port));
    portJsonObj["port_direction"] = "out";
    ports.push_back(portJsonObj);
  }
  toolJsonObj["ports"] = ports;

  return toolJsonObj;
}

void Tool::setPath(const QString& path) {
  Tool::path = path;
}

bool Tool::isEnv() {
  return isEnvTool;
}

void Tool::setIsEnv(bool isEnv) {
  isEnvTool = isEnv;
};

std::unique_ptr<Tool> Tool::create(const QMap<QString, QVariant>& tooldatamap) {
  auto tool = std::make_unique<Tool>();

  QString name = tooldatamap["toolname"].toString();

  /**
    * If pace3d tools are called with a relative or absolute path, e.g.
    * ../bin/volume --xmlhelp
    * then they print their name exactly as called:
    * <program name="../bin/volume" ...>
    * So cut off the path if there is one
    */
  if (name.contains("/")) {
    name = name.split("/").last();
    tool->setName(name);
  } else {
    tool->setName(name);
  }

  tool->setDescription(tooldatamap["description"].toString());
  tool->setExample(tooldatamap["example"].toString());
  tool->setVersion(tooldatamap["version"].toString());
  tool->setIsEnv((tooldatamap.contains("env") && tooldatamap["env"].toBool()));

  // set the tool path
  QString toolPath = tooldatamap["toolpath"].toString(); // the path is not part of the XML
  if (toolPath.isEmpty()) {
    tool->setPath(tool->getName()); // no specific toolpath, so the tool is callable via $PATH and it's name is the path
  } else {
    tool->setPath(toolPath);
  }

  // TODO move this into ToolNode + let ToolNodes sort their ports internally
  if (!tool->isEnv()) {
    auto dependenciesPort = std::make_unique<ToolParameter>();
    dependenciesPort->setLongName("Dependencies");
    dependenciesPort->setType("dependency");
    tool->addInputParameter(std::move(dependenciesPort));

    auto dependentsPort = std::make_unique<ToolParameter>();
    dependentsPort->setLongName("Dependents");
    dependentsPort->setType("dependency");
    tool->addOutputParameter(std::move(dependentsPort));
  }

  QList<QVariant> allarguments = tooldatamap["toolarguments"].toList();

  for (auto parametervariant : allarguments) {
    auto parameter = std::make_unique<ToolParameter>(parametervariant.toJsonObject());

    tool->addInputParameter(std::move(parameter));
  }

  if (tool->isEnv()) {
    auto envOutPort = std::make_unique<ToolParameter>();
    envOutPort->setLongName("environment");
    envOutPort->setType("env");
    tool->addOutputParameter(std::move(envOutPort));
  } else {
    auto envInPort = std::make_unique<ToolParameter>();
    envInPort->setLongName("environment");
    envInPort->setType("env");
    tool->addInputParameter(std::move(envInPort));

    auto stdinPort = std::make_unique<ToolParameter>();
    stdinPort->setLongName("stdin");
    stdinPort->setType("pipe");
    tool->addInputParameter(std::move(stdinPort));

    auto stdoutPort = std::make_unique<ToolParameter>();
    stdoutPort->setLongName("stdout");
    stdoutPort->setType("pipe");
    tool->addOutputParameter(std::move(stdoutPort));
  }

  return tool;
}
