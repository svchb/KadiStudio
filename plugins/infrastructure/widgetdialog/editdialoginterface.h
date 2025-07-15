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

#include <framework/pluginframework/pluginclientinterface.h>

#include <QList>
#include <QString>
class QIcon;
class AbstractValueTypeInterface;


/**
 * @brief      Interface for EditDialogs for easier editing of certain Properties
 * @ingroup    widgetdialog
 */
class EditDialogInterface : public LibFramework::PluginClientInterface {
  public:
    virtual ~EditDialogInterface() = default;

    virtual QString getName() = 0;
    virtual QIcon* getIcon() = 0;

    virtual int exec(AbstractValueTypeInterface* avti) = 0;

    virtual QList<const std::type_info*> getUsableTypes() = 0;

};
