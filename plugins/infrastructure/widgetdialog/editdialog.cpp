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

#include <QIcon>

#include <framework/pluginframework/pluginmanager.h>

#include <plugins/infrastructure/qpropertywidgetfactory/qvtiwidget.h>
#include <plugins/infrastructure/qpropertywidgetfactory/widgetfactoryinterface.h>

#include "editdialog.h"

/** @brief Creates a new EditDialog.
  *
  */
EditDialog::EditDialog()
    : QDialog(),
      icon(nullptr),
      avti(nullptr) {
  this->setModal(true);
}

/** @brief returns icon
  */
QIcon* EditDialog::getIcon() {
  return icon;
}


/** @brief creates Widget matching the value type inteface.
  */
QVTIWidget* EditDialog::createWidget(AbstractValueTypeInterface* avti) {
  auto pluginmanager = LibFramework::PluginManager::getInstance();
  auto *widgetfactory = pluginmanager->getInterface<WidgetFactoryInterface*>("/plugins/infrastructure/qpropertywidgetfactory");

  return widgetfactory->createWidget<QVTIWidget>(avti);
}
