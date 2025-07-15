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

#include <QObject>
#include <QWidget>
#include <QScrollArea>

#include <properties/ui/factory/propertypanel.h>

#include "../qwidgetinterfaceimpl.h"

/**
 * @brief Panel representation with Qt. Every panel in Qt has a corresponding QWidget object.
 *
 * For grouping panels usually QGroupBox is used.
 */
class QtPropertyPanel : public PropertyPanel, public QWidgetInterfaceImpl {
  public:
    /**
     * Creates a direct or indirect sub panel of a top panel.
     */
    QtPropertyPanel(AbstractValueTypeInterface* vti, QWidget* widget);
    virtual ~QtPropertyPanel() = default;

    AbstractValueTypeInterface* getValueTypeInterface() const override {
      return VTIWidget::getValueTypeInterface();
    }

    void setDisabled(bool disabled) override;
    void addWidget(VTIWidget* widget) override;

    void nextColumn() override;
    void nextRow() override;

  private:
    int main_row{};
    int main_column{};

    int column_current_row{};

    std::vector<QWidget*> columns{};
};
