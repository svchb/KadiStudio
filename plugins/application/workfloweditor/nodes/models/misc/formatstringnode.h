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

#include <QtWidgets/QLineEdit>

#include "nodes/data/genericnodedata.h"
#include "plugins/application/workfloweditor/nodes/models/workflownode.h"


/**
 * @brief      Node to create a formatted string using multiple input values
 * @ingroup    misc
 *
 * @note deriving from SourceNode is useful to inherit the 'value' and associated the interface/functionality,
 * those reusable parts could be drawn out of SourceNode in the future, maybe into a mixin
 */
class FormatStringNode : public WorkflowNode {
  Q_OBJECT

  public:
    FormatStringNode();

    QString caption() const override;
    unsigned int nPorts(QtNodes::PortType portType) const override;
    QString portCaption(QtNodes::PortType, QtNodes::PortIndex) const override;
    QString name() const override;

    QJsonObject save() const override;
    void load(QJsonObject const &p) override;

    NodeDataType dataType(PortType portType, PortIndex portIndex) const override;
    QWidget *embeddedWidget() override;

    bool init() override;

  protected:
    QString generateDefaultValue() const;

  private:
    static const unsigned int maxinputs = 100;
    static const unsigned int mininputs = 1;
    static const unsigned int defaultnumberofinputs = 4;
    unsigned int numberofinputs = 0;

    QLineEdit *lineedit;
};
