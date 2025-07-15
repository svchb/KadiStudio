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

#include <QWidget>

#include <framework/pluginframework/pluginmanagerinterface.h>

#include "../propertyformwidgetinterface.h"

class PluginManagerInterface;
class Ambassador;

/**
 * @brief      Provides configuration and information widgets for a specified tool
 * @ingroup    tooldialog
 */
class PropertyFormWidget : public QWidget, public PropertyFormWidgetInterface {
    Q_OBJECT

  public:
    PropertyFormWidget(LibFramework::PluginManagerInterface* pluginmanager);
    ~PropertyFormWidget() override = default;

    QWidget* getWidget() override;

    void createWidgets(Ambassador* ambassador) override;

    void reset() override;

  private:
    LibFramework::PluginManagerInterface *pluginmanager;
};
