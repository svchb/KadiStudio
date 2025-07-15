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
 * @brief A panel for VTIs of type ValueTypeInterfaceContainer
 *
 * For grouping panels usually QGroupBox is used.
 */
class QVTIWidget_ContainerPanel : public PropertyPanel, public QWidgetInterfaceImpl {
  public:
    /**
     * Creates a direct or indirect sub panel of a top panel.
     */
    QVTIWidget_ContainerPanel(AbstractValueTypeInterface* vti, bool issubcontainer, QWidget* widget/*, QWidget* parent = nullptr*/);
    virtual ~QVTIWidget_ContainerPanel() = default;

    AbstractValueTypeInterface* getValueTypeInterface() const override {
      return VTIWidget::getValueTypeInterface();
    }

    void addWidget(VTIWidget* widget) override;

    void nextColumn() override;
    void nextRow() override;

  private:
    int main_row{};
    int main_column{};

    int column_current_row{};

    std::vector<QWidget*> columns{};
};
