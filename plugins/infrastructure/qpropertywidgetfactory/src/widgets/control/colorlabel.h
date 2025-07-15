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

#include <QLabel>
#include <QColor>
#include <QPixmap>
#include <QColorDialog>


/**
 * @class      A label showing the current backgroundcolor.
 * @ingroup    qtpropertywidgetfactory
 */
class ColorLabel : public QLabel {
    Q_OBJECT

  public:
    ColorLabel(QWidget* parent = NULL);
    virtual ~ColorLabel();

    void setSize(int width, int height);

    QColor getColor();
    void setColor(QColor color);

  Q_SIGNALS:
    void colorChanged(QColor color);

  private Q_SLOTS:
    void colorSelected(const QColor& color);

  protected:
    void mousePressEvent(QMouseEvent* event);

  private:

    QPixmap pixmap;
    QColor color;
    QColorDialog colordialog;

};
