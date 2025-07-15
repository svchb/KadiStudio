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

#include <QString>
#include <QVector>
#include <QDate>

#include "src/tooldata/toolparameter.h"


/**
 * @brief      Datastructure for information about a tool.
 * @ingroup    tooldialog
 */
class ToolDescriptionInterface {

  public:
    virtual ~ToolDescriptionInterface() = default;

    virtual void setName(const QString &name) = 0;
    virtual const QString& name() const = 0;
    virtual void setShortName(const QString& shortname) = 0;
    virtual const QString& shortName() const = 0;
    virtual void setVersion(const QString& version) = 0;
    virtual const QString& version() const = 0;
    virtual const QDate& releaseDate() const = 0;
    virtual void setDescription(const QString& description) = 0;
    virtual const QString& description() const = 0;
    virtual void setExample(const QString& example) = 0;
    virtual const QString& example() const = 0;

    virtual void addParameter(const ToolParameter& param) = 0;
    virtual void setParameterVector(const QVector<ToolParameter>& parametervector) = 0;
    virtual const QVector<ToolParameter>& parameterVector() const = 0;
    virtual const QString& getToolpath() const = 0;
    virtual void setCommand(const QString& command) = 0;

    virtual void setEnv(bool isenv) = 0;
    virtual bool isEnv() const = 0;

    virtual bool isValid() const = 0;

};
