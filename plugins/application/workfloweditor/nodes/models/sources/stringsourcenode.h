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

#include "plugins/application/workfloweditor/nodes/models/workflownode.h"
#include "sourcenode.h"


/**
 * @brief      StringSourceNode model
 * @ingroup    sources
 */
class StringSourceNode : public SourceNode {
  Q_OBJECT

  public:
    StringSourceNode();

    QString caption() const override;
    QString name() const override;

    void load(QJsonObject const& p) override;

    NodeDataType dataType(PortType portType, PortIndex portIndex) const override;
    QWidget* embeddedWidget() override;

  protected:
    void setValue(const QString& value) override;
    QString getDefaultValue() const override;

  private Q_SLOTS:
    void onTextEdited(QString const& string);

  private:
    QLineEdit *lineedit;
};
