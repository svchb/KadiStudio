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

#include <framework/pluginframework/pluginmanagerinterface.h>

#include "editdialog_matrix.h"
#include "matrixdialogplugin.h"

void MatrixDialogPlugin::run() {
}

void MatrixDialogPlugin::load() {
  editdialog_matrix = new EditDialog_Matrix();
}

void MatrixDialogPlugin::unload() {
  delete(editdialog_matrix);
}

LibFramework::InterfaceContainer* MatrixDialogPlugin::createInterfaces() {
  return new LibFramework::InterfaceContainer(editdialog_matrix);
}
