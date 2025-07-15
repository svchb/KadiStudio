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

#include "propertypanel.h"
#include "../../data/valuetypeinterfacehint.h"


PropertyPanel::PropertyPanel(AbstractValueTypeInterface* vti) : VTIWidget(vti) {
  const ValueTypeInterfaceHint *hint = vti->getHint();
  if (hint->hasEntry("layout_type")) {
    std::string layout_str = hint->getEntry("layout_type");
    if (layout_str == "grid") {
      layout_type = GRID;
    } else if (layout_str == "vertical") {
      layout_type = VERTICAL;
    } else if (layout_str == "horizontal") {
      layout_type = HORIZONTAL;
    }
  }
}

PropertyPanel::~PropertyPanel() {
  clear();
}

VTIWidget* PropertyPanel::searchWidget(PropertyPanel* panel, const std::string& searchkey) {
  for (auto vtiwidget : panel->getVTIWidgets()) {
    Property *property = dynamic_cast<Property*>(vtiwidget->getValueTypeInterface());
    if (property == nullptr) continue;
    if (searchkey == property->getName()) {
      return vtiwidget;
    }
  }
  return nullptr;
}
