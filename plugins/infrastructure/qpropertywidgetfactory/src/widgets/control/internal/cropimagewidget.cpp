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

#include "cropimagewidget.h"
#include <QDebug>

CropImageWidget::CropImageWidget(QWidget *parent)
  : QLabel(parent), imageLoaded(true), imageCropSelectionRubberBand(new QRubberBand(QRubberBand::Rectangle, this)) {

  // selection rubber band
  QPalette pal;
  pal.setBrush(QPalette::Highlight, QBrush(Qt::white));
  imageCropSelectionRubberBand->setPalette(pal);

  connect(this, &CropImageWidget::mousePressEvent, this, &CropImageWidget::onMousePressEvent);
  connect(this, &CropImageWidget::mouseMoveEvent, this, &CropImageWidget::onMouseMoveEvent);
  connect(this, &CropImageWidget::mouseReleaseEvent, this, &CropImageWidget::onMouseReleaseEvent);
}

void CropImageWidget::onMousePressEvent(QMouseEvent *event) {
  if (imageLoaded) {
    emit selectionRemoved();
    imageCropSelectionOrigin = event->pos();
    imageCropSelectionRubberBand->setGeometry(QRect(imageCropSelectionOrigin, QSize()));
    imageCropSelectionRubberBand->show();
  }
}

void CropImageWidget::onMouseMoveEvent(QMouseEvent *event) {
  if (imageLoaded) {
    auto newGeometry = QRect(imageCropSelectionOrigin, event->pos()).normalized();

    imageCropSelectionRubberBand->setGeometry(newGeometry);
    emit selectionChanged(getSelection());
  }
}

void CropImageWidget::onMouseReleaseEvent(QMouseEvent *event) {
  if (imageLoaded) {
    imageCropSelectionEnd = event->pos();
  }
}

QRect CropImageWidget::getSelection() {
  if (!imageCropSelectionRubberBand) {
    return {};
  }
  QRect selectionRect = imageCropSelectionRubberBand->geometry().normalized();
  if (selectionRect.x() < 0) selectionRect.setX(0);
  if (selectionRect.bottomRight().x() > width()) {
    QPoint newBottomRight = selectionRect.bottomRight();
    newBottomRight.setX(width());
    selectionRect.setBottomRight(newBottomRight);
  }
  if (selectionRect.y() < 0) selectionRect.setY(0);
  if (selectionRect.bottomRight().y() > height()) {
    QPoint newBottomRight = selectionRect.bottomRight();
    newBottomRight.setY(height());
    selectionRect.setBottomRight(newBottomRight);
  }
  return selectionRect;
}

void CropImageWidget::setSelection(int x, int y, int width, int height) {
  if (x == -1 && y == -1 && width == -1 && height == -1) {
    return;
  }
  auto newGeometry = QRect(x, y, width, height).normalized();

  imageCropSelectionRubberBand->setGeometry(newGeometry);
  imageCropSelectionRubberBand->show();
  emit selectionChanged(getSelection());
}

void CropImageWidget::setUrl(QString path) {
  QImage image;
  if (image.load(path)) {
    setText("");
    imageLoaded = true;
  } else {
    setText("no image loaded");
    imageLoaded = false;
    emit selectionRemoved();
    if (imageCropSelectionRubberBand) imageCropSelectionRubberBand->hide();
  }
}
