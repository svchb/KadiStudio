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

static const char *const IMAGE_PATH_REGEX = "%([0-9]+)ld";

#include <QtWidgets/QWidget>
#include <QtWidgets/QSlider>
#include <QGraphicsPixmapItem>
#include <QtWidgets/QLabel>
#include <QtWidgets/QRubberBand>
#include <QtWidgets/QVBoxLayout>

#include "internal/cropimagewidget.h"
#include "../../../qpropertywidget.h"


/**
 * @brief      A widget to perform a crop operation on an image stack with a slider
 * @ingroup    qtpropertywidgetfactory
 */
class CropImageStackWidget : public QPropertyWidget {
    Q_OBJECT

  public:
    CropImageStackWidget(Property* property, QWidget* parent = nullptr);
    ~CropImageStackWidget() override = default;

    void synchronizeVTI() override;

    bool setImageStackPath(const QString &value);

  public Q_SLOTS:
    void onSliderValueChanged(int newValue);
    void onSelectionChanged(QRect newSelection);
    void onSelectionRemoved();

  Q_SIGNALS:
    void imageChanged(QString newUrl);

  private:
    QVBoxLayout *layout;
    // builds url from 'path' and a number
    void setImage(QString path);
    void scanImages();

    int numberOfDigits;
    QString imageStackPath; // must contain a pattern like "%04ld"
    CropImageWidget *imageView;
    QLabel *selectionInfoLabel;
    QSlider *imageSlider;
    std::vector<QString> imageFileNames;

    // helpers
    static QString selectionToString(const QRect &rect);

    bool setImageStackWithPattern(const QString &path);
};
