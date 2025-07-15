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

class LocalFileDialog;

/**
 * @class      Provides a dialog in which the user is able to select a file from the local file system
 * @ingroup    localfiledialog
 */
class LocalFileDialogPlugin : public LibFramework::PluginInterface {

public:
  void run() override;
  void load() override;
  void unload() override;

  LibFramework::InterfaceContainer* createInterfaces() override;

private:
  LocalFileDialog *localFileDialog;

};

PLUGIN_INSTANCE(LocalFileDialogPlugin)
PLUGIN_AUTHORS(Matthieu Laqua)
PLUGIN_NAME(&Local file)
PLUGIN_DESCRIPTION(Provides a dialog in which the user is able to select a file from the local file system)
PLUGIN_NAMESPACE(/plugins/infrastructure/dialogs/fileopen/localfiledialog)
