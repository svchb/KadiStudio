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

#include <cpputils/singleton.hpp>
#include "tabinterface.h"


/**
 * @brief      Delegates the adding of a tab to the gui thread.
 * @ingroup    framework
 */
class DLLAPI TabDelegate : public TabInterface, public Singleton<TabDelegate> {

  friend class Singleton<TabDelegate>;

  public:
    void addTab(const QString& callernamespace, QWidget* widget, const QString& name);
    void addToolBar(const QString& callernamespace, QToolBar* toolbar);
    void setTabName(const QString& callernamespace, const QString& text);
    void setActiveTab(const QString& callernamespace);
    bool isTabActive(const QString &callernamespace);

  private:
    TabDelegate() = default;
};
