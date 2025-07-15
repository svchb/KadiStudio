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

#include "../../../qpropertywidget.h"

class QPushButton;
class FilterListWidget;


/**
 * @brief      Manages two lists widget.
 * @ingroup    qtpropertywidgetfactory
 */
class ListsWidget : public QPropertyWidget {
    Q_OBJECT

  public:

    ListsWidget(Property* property, QWidget* parent = nullptr);
    virtual ~ListsWidget();

    void synchronizeVTI() override;

  Q_SIGNALS:

    void redraw();

  private Q_SLOTS:

    void activateSelected();
    void kickSelected();
    void activeAll();
    void kickAll();

  private:

    FilterListWidget *kickedlistwidget;
    FilterListWidget *activelistwidget;

    QPushButton *activate_all_button;
    QPushButton *activate_selected_button;
    QPushButton *kick_selected_button;
    QPushButton *kick_all_button;

};
