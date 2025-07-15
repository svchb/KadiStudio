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

#include <framework/pluginframework/plugininterface.h>

class ProcessManagerInterface;

class ProcessManagerPlugin : public LibFramework::PluginInterface {

  public:

    void run() override;
    void load() override;
    void unload() override;

    LibFramework::InterfaceContainer* createInterfaces() override;

  private:

    ProcessManagerInterface *interface;

};

PLUGIN_INSTANCE(ProcessManagerPlugin)
PLUGIN_AUTHORS(Philipp Zschumme)
PLUGIN_NAME(ProcessManager Plugin)
PLUGIN_DESCRIPTION(Provides easy access to the process manager interface.)
PLUGIN_NAMESPACE(/plugins/infrastructure/workflows/processmanager)
PLUGIN_REQUIRED_NAMESPACES(
  /plugins/infrastructure/workflows/processmanager/workflow
  /plugins/infrastructure/workflows/processmanager/interaction
)
