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

#include "qvtiwidget_long.h"

QVTIWidget_long::QVTIWidget_long(AbstractValueTypeInterface* valueTypeInterface, QWidget* parent)
    : QVTIWidget(valueTypeInterface, new QSpinBox(parent)) {

  connect(getSpinBox(), &QSpinBox::valueChanged, [&](int value) {
    QVTIWidget::setValue((long)value);
  });
}

void QVTIWidget_long::synchronizeVTI() {
  bool oldState = getSpinBox()->blockSignals(true);

  auto hint = getValueTypeInterface()->getHint();
  long limitmin = INT_MIN;
  if (hint->hasEntry("limit_min")) {
    hint->getEntry("limit_min", limitmin);
  }
  getSpinBox()->setMinimum(limitmin);
  long limitmax = INT_MAX;
  if (hint->hasEntry("limit_max")) {
    hint->getEntry("limit_max", limitmax);
  }
  getSpinBox()->setMaximum(limitmax);

  long value = getValue<long>();
  getSpinBox()->setValue(value);

  getSpinBox()->blockSignals(oldState);
}
