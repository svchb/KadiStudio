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
#include <QStringList>
#include <QVector>
#include <QDate>

#include "toolparameter.h"
#include "../../tooldescriptioninterface.h"


/**
 * @brief      Datastructure for information about a tool.
 * @ingroup    tooldialog
 */
class ToolDescription : ToolDescriptionInterface {

  public:
    ToolDescription() = default;

    void setName(const QString &name) override {
      toolname = name;
    }
    const QString& name() const override {
      return toolname;
    }
    void setShortName(const QString& shortname) override {
      toolshortname = shortname;
    }
    const QString& shortName() const override {
      return toolshortname;
    }
    void setVersion(const QString& version) override;
    const QString& version() const override {
      return toolversion;
    }
    const QDate& releaseDate() const override {
      return toolreleasedate;
    }
    void setDescription(const QString& description) override {
      tooldescription = description;
    }
    const QString& description() const override {
      return tooldescription;
    }
    void setExample(const QString& example) override {
      toolexample = example;
    }
    const QString& example() const override {
      return toolexample;
    }

    void addParameter(const ToolParameter& param) override {
      toolparams.push_back(param);
    }
    void setParameterVector(const QVector<ToolParameter>& parametervector) override {
      toolparams.clear();
      for (const auto &parameter : parametervector) {
        toolparams.push_back(parameter);
      }
    }
    const QVector<ToolParameter>& parameterVector() const override {
      return toolparams;
    }
    const QString& getToolpath() const override {
      return command;
    }
    void setCommand(const QString& command) override {
      this->command = command;
    }

    void setEnv(bool isenv) override {
      this->isenv = isenv;
    }
    bool isEnv() const override {
      return isenv;
    }

    void removeHelp();

    bool isValid() const override {
      return not name().isEmpty();
    }

  private:
    QString toolname;
    QString toolversion;
    QDate   toolreleasedate;
    QString tooldescription;
    QString toolexample;
    QString toolshortname;
    QVector<ToolParameter> toolparams;
    QString command;

    bool isenv;

};
