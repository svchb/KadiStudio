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

#include <array>

#include <QLabel>
#include <QVBoxLayout>
#include <QDoubleSpinBox>

#include <properties/data/valuetypeinterfacehint.h>

// #include <wrapper.h>

#include "qvtiwidget_arcball.h"

QVTIWidget_arcball::QVTIWidget_arcball(AbstractValueTypeInterface* valuetypeinterface, QWidget* parent)
    : QVTIWidget(valuetypeinterface, new QGroupBox("Arcball", parent)) {

  QVBoxLayout   *widgetlayout  = new QVBoxLayout(getWidget());

  QHBoxLayout   *xoffsetlayout = new QHBoxLayout();
  QLabel        *xoffsetlabel  = new QLabel("X");
  xoffsetdoublespinbox = new QDoubleSpinBox(getWidget());
  xoffsetlayout->addWidget(xoffsetlabel);
  xoffsetlayout->addWidget(xoffsetdoublespinbox);
  widgetlayout->addLayout(xoffsetlayout);

  QHBoxLayout   *yoffsetlayout = new QHBoxLayout();
  QLabel        *yoffsetlabel  = new QLabel("Y");
  yoffsetdoublespinbox = new QDoubleSpinBox(getWidget());
  yoffsetlayout->addWidget(yoffsetlabel);
  yoffsetlayout->addWidget(yoffsetdoublespinbox);
  widgetlayout->addLayout(yoffsetlayout);

  QHBoxLayout   *zoffsetlayout = new QHBoxLayout();
  QLabel        *zoffsetlabel  = new QLabel("Z");
  zoffsetdoublespinbox = new QDoubleSpinBox(getWidget());
  zoffsetlayout->addWidget(zoffsetlabel);
  zoffsetlayout->addWidget(zoffsetdoublespinbox);
  widgetlayout->addLayout(zoffsetlayout);

  widgetlayout->addStretch(0);

  connect(xoffsetdoublespinbox, SIGNAL(valueChanged(double)), this, SLOT(itemValueInternal(double)));
  connect(yoffsetdoublespinbox, SIGNAL(valueChanged(double)), this, SLOT(itemValueInternal(double)));
  connect(zoffsetdoublespinbox, SIGNAL(valueChanged(double)), this, SLOT(itemValueInternal(double)));

  connect(this, SIGNAL(itemValueChanged(int, double)), this, SLOT(setValue(int, double)));

  const ValueTypeInterfaceHint *hint = valuetypeinterface->getHint();
  int limitmin = -180;
  int limitmax =  180;
  if (hint->hasEntry("limit_min")) {
    hint->getEntry("limit_min", limitmin);
  }
  if (hint->hasEntry("limit_max")) {
    hint->getEntry("limit_max", limitmax);
  }
  setLimits(limitmin, limitmax);
}

void QVTIWidget_arcball::synchronizeVTI() {
  std::array<float, 16> value;
  getValue(value);

  // REAL valueReal[3][3];
  // vec_toReal3f(valueReal[0], value.data());
  // vec_toReal3f(valueReal[1], value.data() + 4);
  // vec_toReal3f(valueReal[2], value.data() + 8);
  // long convention[3] = {X, Y, Z};
  // Matrix3D_getAngleEuler_degree(valueReal, rotation, convention);
}

void QVTIWidget_arcball::setValue(int index, double value) {
  rotation[index] = value;

  std::array<float, 16> rot;
  // REAL rotReal[3][3];
  // Matrix3D_makeRotation(rotReal, rotation);
  // vec_toFloat3r(rot.data(), rotReal[0]);
  // rot[3] = 0.0;
  // vec_toFloat3r(rot.data() + 4, rotReal[1]);
  // rot[7] = 0.0;
  // vec_toFloat3r(rot.data() + 8, rotReal[2]);
  // rot[11] = 0.0;
  // vec_initZeroNf(rot.data() + 12, 4);

  QVTIWidget::setValue(rot);
}

void QVTIWidget_arcball::setLimits(double min, double max) {
  xoffsetdoublespinbox->setRange(min, max);
  yoffsetdoublespinbox->setRange(min, max);
  zoffsetdoublespinbox->setRange(min, max);
}

void QVTIWidget_arcball::itemValueInternal(double value) {
  if (sender() == xoffsetdoublespinbox) {
    emit itemValueChanged(0, value);
  } else if (sender() == yoffsetdoublespinbox) {
    emit itemValueChanged(1, value);
  } else if (sender() == zoffsetdoublespinbox) {
    emit itemValueChanged(2, value);
  }
}
