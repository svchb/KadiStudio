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

#include <QtCore/QString>
#include <QtWidgets/QWidget>
#include <QtCore/QVariant>

#include "../interactioninterface.h"
#include "../interactiondirection.h"

/**
 * @brief      A class to represent instances of user interactions issued
 *             by the process engine while a workflow is executed.
 * @ingroup    src
 */
class Interaction : public InteractionInterface {

  public:
    std::unique_ptr<InteractionInterface> create() const override;

    const QString& getTypeIdentifier() const override;
    void fromJson(QJsonObject jsonObj) override;
    const QString& getUrl() const override;
    const QString& getPrompt() const override;
    const QVariant& getDefaultValue() const override;
    const QVariant& getValue() const override;
    int getOrder() const override;
    const QString& getId() const override;
    int getPageNumber() const override;
    InteractionDirection getDirection() const override;
    void setValue(const QVariant& value) override;
    const std::vector<QString>& getOptions() const override;
    bool isMultiline() const override;

  private:
    QString id;
    InteractionDirection direction;
    QString typeidentifier;
    int pageNumber;
    int order;

    QString prompt;
    QVariant default_value;
    QVariant value;
    QString url;
    std::vector<QString> options;
    bool multiline;
};

