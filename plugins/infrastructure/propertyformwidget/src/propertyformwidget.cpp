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

#include <QVBoxLayout>
#include <QDebug>

#include <plugins/infrastructure/qpropertywidgetfactory/widgetfactoryinterface.h>

#include "propertyformwidget.h"

PropertyFormWidget::PropertyFormWidget(LibFramework::PluginManagerInterface* pluginmanager)
    : pluginmanager(pluginmanager) {

  setMinimumWidth(500); // to make input fields visible in grid layout
}

void PropertyFormWidget::createWidgets(Ambassador* ambassador) {
  if (!ambassador) {
    qDebug() << "Error: Cannot create widgets. Ambassador is null!";
    return;
  }
  auto *factory = pluginmanager->getInterface<WidgetFactoryInterface*>("/plugins/infrastructure/qpropertywidgetfactory");

  this->setLayout(new QVBoxLayout());
  factory->createScollableGui(ambassador, this);
}

QWidget* PropertyFormWidget::getWidget() {
  return this;
}

void PropertyFormWidget::reset() {
  Q_FOREACH (QObject* child, this->children()) {
    delete child;
  }
}
