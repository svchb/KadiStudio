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

#include <QWidget>
#include <QSpinBox>

#include "../../qvtiwidget.h"


/**
 * @brief      A spinbox widget for long values with a property.
 * @ingroup    qtwidgetfactory
 */
class QVTIWidget_long : public QVTIWidget {
  Q_OBJECT

  public:

    QVTIWidget_long(AbstractValueTypeInterface* valuetypeinterface, QWidget* parent = NULL);
    virtual ~QVTIWidget_long() = default;

    void synchronizeVTI() override;

  private:
    virtual bool validateValue() override {return true;};

    QSpinBox* getSpinBox() {
      return dynamic_cast<QSpinBox*>(getWidget());
    }

};
