/* Copyright 2022 Karlsruhe Institute of Technology
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
#include "dockinterface.h"


/**
 * @brief      Delegates the adding of a dock window to the gui thread.
 * @ingroup    framework
 */
class DLLAPI DockDelegate : public DockInterface, public Singleton<DockDelegate> {

  friend class Singleton<DockDelegate>;

  public:
    void addDockWindow(const QString& callernamespace, DockWindow* dockwindow) override;

    void dockWindow(const QString& callernamespace, DockWindow* dockwindow, bool dock) override;

  private:
    DockDelegate() = default;
};
