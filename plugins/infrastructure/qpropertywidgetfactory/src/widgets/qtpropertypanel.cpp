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

#include "../qwidgetinterfaceimpl.h"

#include "qtpropertypanel.h"
#include "optionalcheckbox.h"
#include "properties/data/valuetypeinterface/abstractvaluetypeinterface.h"

class ResetButton : public QPushButton {
  public:

    ResetButton(QWidgetInterface* qvtiwidget) : QPushButton() {
      AbstractValueTypeInterface *watching_vti = qvtiwidget->getValueTypeInterface();
      const ValueTypeInterfaceHint *hint = watching_vti->getHint();
      assert(hint->hasEntry("default"));

      // Keep size even if not visible
      QSizePolicy sp_retain = this->sizePolicy();
      sp_retain.setRetainSizeWhenHidden(true);
      this->setSizePolicy(sp_retain);

      int iconSize = this->style()->pixelMetric(QStyle::PM_SmallIconSize, nullptr, this);
      this->setFixedWidth(iconSize);

      this->setIcon(this->style()->standardIcon(QStyle::SP_BrowserReload));
      this->setToolTip(tr("Reset to default value\n%1").arg(QString::fromStdString(hint->getEntry("default"))));
      this->setIconSize(QSize(iconSize, iconSize));
      this->setFocusPolicy(Qt::StrongFocus);
      this->setStyleSheet("QPushButton { border: none; padding: 3px; }");

      connect(this, &QPushButton::clicked, [qvtiwidget]() {
        qvtiwidget->getWidget()->setFocus();
        qvtiwidget->resetDefault();
      });

      event_handles += watching_vti->onChange([this](AbstractValueTypeInterface* avti) {
        toggle(avti);
      });

      toggle(watching_vti);
    }

    void toggle(AbstractValueTypeInterface* watching_vti) {
      const ValueTypeInterfaceHint *hint = watching_vti->getHint();
      if (watching_vti->compareToString(hint->getEntry("default"))) {
        this->hide();
      } else {
        this->show();
      }
    }

  private:
    EventHandles event_handles{};
};

QtPropertyPanel::QtPropertyPanel(AbstractValueTypeInterface* vti, QWidget* widget)
    : PropertyPanel(vti), QWidgetInterfaceImpl(widget) {
  getWidget()->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);

  auto grid_layout = static_cast<QGridLayout*>(getWidget()->layout());
  grid_layout->setAlignment(Qt::AlignTop);

  connect(getWidget(),   SIGNAL(lostFocus()),                   this, SIGNAL(lostFocus()));

  nextColumn();
}

void QtPropertyPanel::setDisabled(bool disabled) {
  getWidget()->setDisabled(disabled);
}

void QtPropertyPanel::addWidget(VTIWidget* vtiwidget) {
  static constexpr int COLUMN_OPTIONAL = 0;
  static constexpr int COLUMN_LABEL    = 1;
  static constexpr int COLUMN_DEFAULT  = 2;
  static constexpr int COLUMN_WIDGET   = 3;

  PropertyPanel::addWidget(vtiwidget);

  auto qwi = dynamic_cast<QWidgetInterface*>(vtiwidget);
  auto vti = vtiwidget->getValueTypeInterface();
  auto hint = vti->getHint();

  auto grid_layout = static_cast<QGridLayout*>(columns.back()->layout());

  const auto label_pos = hint->getEntry("label.pos");

  // Local function to add a label.
  const auto add_label = [&](int column) {
    if (label_pos != "none" && hint->hasEntry("label")) {
      auto label = new QLabel(QString::fromStdString(hint->getEntry("label")));

      label->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
      if (dynamic_cast<PropertyPanel*>(vtiwidget)) {
        grid_layout->addWidget(label, column_current_row, column, Qt::AlignTop);
      } else {
        grid_layout->addWidget(label, column_current_row, column);
      }
    }
  };

  // Position the label one row above the actual widget.
  if (label_pos == "top") {
    add_label(COLUMN_WIDGET);
    ++column_current_row;
  }

  if (hint->hasEntry("optional") && hint->getEntry<bool>("optional")) {
    auto optional_checkbox = new OptionalCheckbox(qwi);

    grid_layout->addWidget(optional_checkbox, column_current_row,  COLUMN_OPTIONAL);
  }

  // Position the label in the same row as the widget.
  if (label_pos != "top") {
    add_label(COLUMN_LABEL);
  }

  if (hint->hasEntry("default")) {
    auto reset_button = new ResetButton(qwi);
    grid_layout->addWidget(reset_button, column_current_row, COLUMN_DEFAULT);
  }

  qwi->getOuterWidget()->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
  grid_layout->addWidget(qwi->getOuterWidget(), column_current_row, COLUMN_WIDGET);

  ++column_current_row;
}

void QtPropertyPanel::nextColumn() {
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

void QtPropertyPanel::nextRow() {
  main_column = 0;
  ++main_row;

  nextColumn();
}
