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

#include <QPushButton>

#include <properties/data/properties/functionproperty.h>

#include "qpropertypushbutton.h"

QPropertyPushButton::QPropertyPushButton(Property* property, QWidget* parent)
    : QPropertyWidget(property, new QPushButton(QString::fromStdString(property->getName()), parent)) {
  connect(reinterpret_cast<QPushButton*>(getWidget()), &QPushButton::clicked, [&]() {
    getProperty<FunctionProperty>()->callUpdateFunction();
  });
}

void QPropertyPushButton::synchronizeVTI() {
}
