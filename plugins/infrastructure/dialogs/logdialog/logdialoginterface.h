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

class QString;

class LogDialogInterface : public LibFramework::PluginClientInterface {

public:
  /**
   * Open a new log dialog displaying the log for the workflow with given id
   * @param workflow_id Id of the workflow for which the log should be displayed
   */
  virtual void showLogDialog(int workflow_id) = 0;
  /**
   * Function to check if the log dialog is open.
   * @return true if log dialog is open (visible), false otherwise.
   */
  virtual bool isOpen() = 0;
};
