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
#include "plugins/infrastructure/toolchooser/src/tooldata/tooldescription.h"


/**
 * @brief      ToolChoosers' public interface for other plugins.
 * @ingroup    toolchooser
 */
class ToolChooserInterface : public LibFramework::PluginClientInterface {

  public:
    virtual ToolChooserInterface* clone() const = 0;

    virtual QWidget* getWidget() = 0;

    virtual void setTool(const QString& toolidentificationstring) = 0;

    virtual const ToolDescription& getToolDescription(const QString& toolidentificationstring) = 0;
    virtual const ToolDescription& getToolDescription() const = 0;
    virtual QMap<QString, QVariant> getToolData() = 0;

    virtual void setToolSelectedCallback(std::function<void()> function) = 0;
    virtual void setToolResetCallback(std::function<void()> function) = 0;

};
