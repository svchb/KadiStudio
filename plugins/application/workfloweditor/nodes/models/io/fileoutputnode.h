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

#include <QtCore/QObject>
#include <QtWidgets/QCheckBox>

#include "nodes/models/workflownode.h"


/**
 * @brief      FileOutputNode model
 * @ingroup    io
 */
class FileOutputNode : public WorkflowNode {
  Q_OBJECT

public:
  FileOutputNode();

  QString caption() const override;
  QString name() const override;
  unsigned int nPorts(PortType portType) const override;
  NodeDataType dataType(PortType portType, PortIndex portIndex) const override;
  QString portCaption(PortType portType, PortIndex portIndex) const override;
  QWidget * embeddedWidget() override;
  ConnectionPolicy portConnectionPolicy(PortType portType, PortIndex portIndex) const override;

  QJsonObject save() const override;
  void load(QJsonObject const &p) override;

private Q_SLOTS:
  void onShortcutStateChanged(bool newState);

private:
  bool createShortcut;
  QCheckBox* shortcutCheckbox;

};
