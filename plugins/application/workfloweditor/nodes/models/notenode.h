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

#include <QPlainTextEdit>
#include <QDebug>
#include <QtNodes/NodeDelegateModel>
#include "plugins/application/workfloweditor/config.h"
#include "workflownode.h"

using QtNodes::NodeDelegateModel;
using QtNodes::NodeDataType;
using QtNodes::NodeStyle;


/**
 * @brief      A node to display a text note / comment in the workflow
 * @ingroup    models
 */
class NoteNode : public WorkflowNode {
  Q_OBJECT

  public:
    NoteNode();

    QString caption() const override {
      return "";
    }

    bool captionVisible() const override {
      return false;
    }

    QString name() const override {
      return "Note";
    }

    unsigned int nPorts(PortType) const override {
      return 0;
    }

    NodeDataType dataType(PortType, PortIndex) const override {
      return {};
    }

    QWidget* embeddedWidget() override {
      return textfield;
    }

    QJsonObject save() const override;

    void load(QJsonObject const &p) override;

    bool resizable() const override { return true; }

  public Q_SLOTS:
    void updateHeight();
    /**
     * This slot allows to trigger line wrapping explicitly to get the correct number of lines in updateHeight()
     * afterwards.
     */
    void syncLines();

  private:
    QPlainTextEdit *textfield;

    int fontheight; /// Height in pixels that one line of text is expected to occupy
    const static int VERTICAL_SPACE = 20; /// Fixed additional vertical space to avoid that a scrollbar is displayed
    const static int FONTHEIGHT_EXT = 1;
};
