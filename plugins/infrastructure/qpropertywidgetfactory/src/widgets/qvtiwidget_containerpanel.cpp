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

#include <QtWidgets>

#include <properties/ui/factory/vtiwidget.h>

#include "../../qwidgetinterface.h"

#include "qvtiwidget_containerpanel.h"
// #include "properties/data/valuetypeinterface/abstractvaluetypeinterface.h"

QVTIWidget_ContainerPanel::QVTIWidget_ContainerPanel(AbstractValueTypeInterface* vti, bool issubcontainer, QWidget* widget)
    : PropertyPanel(vti), QWidgetInterfaceImpl(widget) {
  getWidget()->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);

  auto grid_layout = static_cast<QGridLayout*>(getWidget()->layout());
  grid_layout->setAlignment(Qt::AlignTop);

  QGroupBox *groupbox = dynamic_cast<QGroupBox*>(widget);

  if (issubcontainer) {
    groupbox->setFlat(true);
    // connect(this, &QVTIWidget_ContainerPanel::gotFocus, [=]() {
    //
    // });

  } else {
    // auto scroll_area = new QScrollArea(/*groupbox*/);
    // scroll_area->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    // scroll_area->setWidgetResizable(true);
    // scroll_area->setWidget(groupbox);

  }


  nextColumn();
}

void QVTIWidget_ContainerPanel::addWidget(VTIWidget* vtiwidget) {

  PropertyPanel::addWidget(vtiwidget);

  auto qwi = dynamic_cast<QWidgetInterface*>(vtiwidget);

  auto grid_layout = static_cast<QGridLayout*>(columns.back()->layout());

  qwi->getOuterWidget()->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
  grid_layout->addWidget(qwi->getOuterWidget(), column_current_row, main_column);

  ++column_current_row;
}

void QVTIWidget_ContainerPanel::nextColumn() {
  auto main_grid_layout = static_cast<QGridLayout*>(getWidget()->layout());

  auto column_grid_layout = new QGridLayout();
  column_grid_layout->setAlignment(Qt::AlignTop);

  auto column_widget = new QWidget(getWidget());
  column_widget->setLayout(column_grid_layout);
  column_widget->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);

  main_grid_layout->addWidget(column_widget, main_row, main_column++);
  columns.push_back(column_widget);

  // Reset the row cursor for the current column.
  column_current_row = 0;
}

void QVTIWidget_ContainerPanel::nextRow() {
  main_column = 0;
  ++main_row;

  nextColumn();
}
