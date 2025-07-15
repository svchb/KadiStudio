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

#include <QGroupBox>
#include "../../../qpropertywidget.h"

class QComboBox;


/**
 * @class      A widget for controlling the bordermode.
 * @ingroup    propertywidgetfactory
 */
class BorderModeWidget : public QPropertyWidget {
    Q_OBJECT

  public:
    BorderModeWidget(Property* property, QWidget* parent = NULL);
    virtual ~BorderModeWidget();

    void synchronizeVTI() override;

  Q_SIGNALS:
    void redraw();

  private Q_SLOTS:
    void setBorderModeLeft(int mode);
    void setBorderModeBottom(int mode);
    void setBorderModeBack(int mode);
    void setBorderModeRight(int mode);
    void setBorderModeTop(int mode);
    void setBorderModeFront(int mode);

  private:

    QComboBox *leftcombobox;
    QComboBox *bottomcombobox;
    QComboBox *backcombobox;
    QComboBox *rightcombobox;
    QComboBox *topcombobox;
    QComboBox *frontcombobox;

    void setBorderMode(int mode, int side);

};
