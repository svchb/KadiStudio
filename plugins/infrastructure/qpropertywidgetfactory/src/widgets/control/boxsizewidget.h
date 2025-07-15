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

class Ambassador;


/**
 * @brief      GUI to control the bounding box size of the simulation.
 * @ingroup    qtpropertywidgetfactory
 */
class BoxSizeWidget : public QPropertyWidget {
    Q_OBJECT

  public:
    BoxSizeWidget(Property* property, QWidget* parent = nullptr);
    ~BoxSizeWidget() override = default;

    void synchronizeVTI() override;

    void setXRowEnabled(bool enabled);
    void setYRowEnabled(bool enabled);
    void setZRowEnabled(bool enabled);

  Q_SIGNALS:
    void redraw();

  private:
    QWidget* setupDimensionWidgets(Ambassador* ambassador, const char* dim, const std::string& lowername, const std::string& uppername);

    QWidget *xwidget;
    QWidget *ywidget;
    QWidget *zwidget;

};
