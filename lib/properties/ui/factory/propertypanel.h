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

#include "vtiwidget.h"

#include "../../data/ambassador.h"

#include <algorithm>

/**
 * @brief Abstract class for GUI panels which are used to group properties and sub panels of the same domain.
 *
 * When creating a new backend this abstract class has to be inherited to provide custom panels for the backend.
 * Every panel instance should contain a group box or panel of the corresponding GUI framework.
 *
 * The panels map the hierarchy of ambassadors and property domains by providing a relation to their parent property panel.
 */
class PropertyPanel : public VTIWidget {
  public:
    /**
     * Creates a new property panel.
     * @param vti valuetypeinterface
     */
    PropertyPanel(AbstractValueTypeInterface* vti);
    virtual ~PropertyPanel();

    /**
     * Sets disabled status of child widgets
     * @param disabled
     */
    virtual void setDisabled([[maybe_unused]] bool disabled) {};

    /** @brief Add a widget to the panel, respecting the internal layout.
     *
     * To take care of "decorating" the widget, meaning that a checkbox
     * and a label will be added in case the hint (ValueTypeInterfaceHint)
     * specifies it this function must be overloaded.
     *
     * @param widget VTIWidget pointer which must be non-null
     */
    virtual void addWidget(VTIWidget* widget) {
      vtiwidgets.push_back(widget);
    }

    virtual void remove(VTIWidget* widget) {
      vtiwidgets.erase(std::remove(vtiwidgets.begin(), vtiwidgets.end(), widget), vtiwidgets.end());
      delete widget;
    }

    VTIWidget* searchWidget(PropertyPanel* panel, const std::string& searchkey);

    virtual void clear() {
      std::cerr << "clear()xx " << this << std::endl;
      for (size_t i = 0; i < vtiwidgets.size(); ++i) {
        std::cerr << "clear() " << i << std::endl;
        delete vtiwidgets.at(i);
      }

      vtiwidgets.clear();
    }

    void synchronizeVTI() override {
      for (auto iter = vtiwidgets.begin(); iter != vtiwidgets.end(); iter++) {
        (*iter)->synchronizeVTI();
      }
    }

    const std::vector<VTIWidget*>& getVTIWidgets() {
      return vtiwidgets;
    }

    virtual void nextColumn() {}
    virtual void nextRow() {}

  protected:
    /**
     * Internal layout of the PropertyPanel
     */
    enum LayoutType {
      GRID,
      HORIZONTAL,
      VERTICAL
    };
    LayoutType layout_type = LayoutType::VERTICAL;

    std::vector<VTIWidget*> vtiwidgets;
};
