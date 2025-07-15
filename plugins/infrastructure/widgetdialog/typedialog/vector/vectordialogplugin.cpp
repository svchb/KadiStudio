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

#include "editdialog_vector.h"
#include "editdialog_vectorfill.h"
#include "vectordialogplugin.h"

void VectorDialogPlugin::run() {
}

void VectorDialogPlugin::load() {
  editdialog_vector = new EditDialog_Vector();
  editdialog_vectorfill = new EditDialog_VectorFill();
}

void VectorDialogPlugin::unload() {
  delete(editdialog_vectorfill);
  delete(editdialog_vector);
}

LibFramework::InterfaceContainer* VectorDialogPlugin::createInterfaces() {
  return new LibFramework::InterfaceContainer(editdialog_vector, editdialog_vectorfill);
}
