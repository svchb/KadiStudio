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

#include <QCheckBox>

#include "../../qwidgetinterface.h"
#include <properties/data/valuetypeinterfacehint.h>
#include <properties/data/valuetypeinterface/valuetypeinterface.h>


/**
 * @ingroup    widgets
 * @brief      Checkbox to toggle a widget for an optional (non-required) property
 */
class OptionalCheckbox : public QCheckBox {
  Q_OBJECT

  public:
    OptionalCheckbox(QWidgetInterface* widget)
        : QCheckBox(widget->getWidget()) {
      setToolTip(tr("Enable optional parameter"));
      bool checked = false;
      ValueTypeInterfaceHint *hint = widget->getValueTypeInterface()->updateHint();
      if (hint->hasEntry("optional_set")) {
        hint->getEntry("optional_set", checked);
      }
      setChecked(checked);
      connect(this, &QCheckBox::toggled, [widget](bool checked) {
        widget->getWidget()->setEnabled(checked);
        widget->getValueTypeInterface()->updateHint()->setEntry("optional_set", checked);
        widget->getValueTypeInterface()->notify();
      });
      widget->getWidget()->setEnabled(checked);
    }

};
