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

#include <QMouseEvent>

#include "colorlabel.h"

ColorLabel::ColorLabel(QWidget* parent)
    : QLabel(parent) {
  this->setFixedSize(35,35);
  pixmap = QPixmap(35, 35);
  color = Qt::black;
  pixmap.fill(color);
  this->setPixmap(pixmap);
}

ColorLabel::~ColorLabel() {
}

void ColorLabel::setSize(int width, int height) {
  this->setFixedSize(width, height);
  QPixmap pixmap(width, height);
  pixmap.fill(color);
  this->setPixmap(pixmap);
}

QColor ColorLabel::getColor() {
  return color;
}

void ColorLabel::setColor(QColor color) {
  this->color = color;
  pixmap.fill(color);
  this->setPixmap(pixmap);
}

void ColorLabel::mousePressEvent(QMouseEvent* event) {
  if (event->button() & Qt::LeftButton) {
    colordialog.setCurrentColor(color);
    colordialog.open(this, SLOT(colorSelected(const QColor&)));
  }
}

void ColorLabel::colorSelected(const QColor & color) {
  if (this->color == color) {
    return;
  }
  this->color = color;
  pixmap.fill(color);
  this->setPixmap(pixmap);
  emit colorChanged(color);
}
