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

#include <QSpinBox>

#include "qvtiwidget_int.h"

QVTIWidget_int::QVTIWidget_int(AbstractValueTypeInterface* valueTypeInterface, QWidget* parent)
    : QVTIWidget(valueTypeInterface, new QSpinBox(parent)) {

  connect(getSpinBox(), &QSpinBox::valueChanged, this, [&](int value) {
    QVTIWidget::setValue(value);
  });
}

void QVTIWidget_int::synchronizeVTI() {
  auto hint = getValueTypeInterface()->getHint();
  int limitmin = INT_MIN;
  if (hint->hasEntry("limit_min")) {
    hint->getEntry("limit_min", limitmin);
  }
  getSpinBox()->setMinimum(limitmin);
  int limitmax = INT_MAX;
  if (hint->hasEntry("limit_max")) {
    hint->getEntry("limit_max", limitmax);
  }
  getSpinBox()->setMaximum(limitmax);

  int value;
  getValue(value);
  getSpinBox()->setValue(value);
}
