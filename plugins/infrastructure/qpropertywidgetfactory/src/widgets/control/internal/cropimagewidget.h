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

#pragma once

#include <QtCore/QArgument>
#include <QtWidgets/QLabel>
#include <QMouseEvent>
#include <QtWidgets/QRubberBand>


class CropImageWidget : public QLabel {
  Q_OBJECT
public:
  CropImageWidget(QWidget* parent = nullptr);

  void setUrl(QString path);
  void setSelection(int x, int y, int width, int height);

Q_SIGNALS:
  void selectionChanged(QRect selection);
  void selectionRemoved();

  void imageMousePressEvent(QMouseEvent* event);
  void imageMouseMoveEvent(QMouseEvent* event);
  void imageMouseReleaseEvent(QMouseEvent* event);

  void mousePressEvent(QMouseEvent* event) override;
  void mouseMoveEvent(QMouseEvent* event) override;
  void mouseReleaseEvent(QMouseEvent* event) override;

private Q_SLOTS:
  void onMousePressEvent(QMouseEvent* event);
  void onMouseMoveEvent(QMouseEvent* event);
  void onMouseReleaseEvent(QMouseEvent* event);

private:
  QRect getSelection();

  bool imageLoaded;
  // crop selection
  QRubberBand *imageCropSelectionRubberBand;
  QPoint imageCropSelectionOrigin;
  QPoint imageCropSelectionEnd;

};
