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

#include <memory>
#include <vector>
#include <framework/pluginframework/pluginclientinterface.h>
#include "interactiondirection.h"

class QString;
class QJsonObject;
class QVariant;

/**
 * @brief      Provides access to an Interaction object
 * @ingroup    interaction
 */
class InteractionInterface : public LibFramework::PluginClientInterface {

  public:
    ~InteractionInterface() override = default;
    virtual std::unique_ptr<InteractionInterface> create() const = 0;

    virtual const QString& getTypeIdentifier() const = 0;
    virtual void fromJson(QJsonObject jsonObj) = 0;
    virtual const QString& getUrl() const = 0;
    virtual const QString& getPrompt() const = 0;
    virtual const QVariant& getDefaultValue() const = 0;
    virtual const QVariant& getValue() const = 0;
    virtual int getOrder() const = 0;
    virtual const QString& getId() const = 0;
    virtual int getPageNumber() const = 0;
    virtual InteractionDirection getDirection() const = 0;
    virtual void setValue(const QVariant& value) = 0;
    virtual const std::vector<QString>& getOptions() const = 0;
    virtual bool isMultiline() const = 0;
};
