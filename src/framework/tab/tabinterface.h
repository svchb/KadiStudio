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
#include <QToolBar>


/**
 * @brief      Tab interface for plugins.
 * @ingroup    libframework
 */
class TabInterface {

  public:
    virtual ~TabInterface() = default;

    virtual void addTab(const QString& callernamespace, QWidget* widget, const QString& name = "Plugin") = 0;

    virtual void addToolBar(const QString& callernamespace, QToolBar* toolbar) = 0;

    virtual void setTabName(const QString& callernamespace, const QString& text) = 0;

    virtual void setActiveTab(const QString& callernamespace) = 0;

    virtual bool isTabActive(const QString& callernamespace) = 0;

};
