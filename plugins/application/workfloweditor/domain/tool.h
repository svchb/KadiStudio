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

#include <memory> // unique_ptr
#include <QtCore/QVector>
#include <QtCore/QJsonObject>
#include <plugins/infrastructure/toolchooser/src/tooldata/toolparameter.h>


/**
 * @brief      A description of a tool which can be parsed from XML (xmlhelp format)
 *             and can be converted into json for the .flow format.
 * @ingroup    domain
 */
class Tool {

  public:
    explicit Tool() = default;
    explicit Tool(const QJsonObject& jsonObject);
    Tool(const Tool&) = delete;
    Tool& operator=(const Tool&) = delete;

    // simple getters and setters

    const QString& getName() const;
    void setName(const QString& name);
    void setPath(const QString& path);
    const QString& getVersion() const;
    void setVersion(const QString& version);
    const QString& getDescription() const;
    void setDescription(const QString& description);
    const QString& getExample() const;
    void setExample(const QString& example);
    bool isEnv();
    void setIsEnv(bool isEnv);

    void addInputParameter(std::unique_ptr<ToolParameter> parameter);
    void addOutputParameter(std::unique_ptr<ToolParameter> parameter);
    int getInputParameterCount() const;
    int getOutputParameterCount() const;

    ToolParameter* getInputParameter(int index) const;
    ToolParameter* getOutputParameter(int index) const;

    QJsonObject toJson();
    static std::unique_ptr<Tool> create(const QMap<QString, QVariant>& tooldatamap);

  private:
    QString name;
    QString version;
    QString example;
    QString description;
    QString path;
    bool isEnvTool;

    std::vector<std::unique_ptr<ToolParameter>> inputParameters;
    std::vector<std::unique_ptr<ToolParameter>> outputParameters;
};
