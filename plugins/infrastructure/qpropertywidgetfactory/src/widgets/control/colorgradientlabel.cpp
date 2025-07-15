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

#include <QPainter>

#include "colorgradientlabel.h"

ColorGradientLabel::ColorGradientLabel(QWidget* parent)
    : QLabel(parent) {
  setFixedSize(100, 120);
}

ColorGradientLabel::~ColorGradientLabel() {
}

void ColorGradientLabel::setColorSteps(const QMap<double, QColor>& colorsteps) {
  gradient = QLinearGradient(0, 0, 0, height());
  QMapIterator<double, QColor> it(colorsteps);
  while (it.hasNext()) {
    it.next();
    double pos = it.key();
    QColor color = it.value();
    gradient.setColorAt(pos, color);
  }
  // revert gradient
  gradient.setStart(0., height()); // bottom right
  gradient.setFinalStop(0., 0.); // top left
  update();
}

void ColorGradientLabel::paintEvent(QPaintEvent* event) {
  Q_UNUSED(event);
  QPainter painter(this);
  painter.setBrush(gradient);
  painter.drawRect(QRect(0, 0, width() - 1, height() - 1));
}
