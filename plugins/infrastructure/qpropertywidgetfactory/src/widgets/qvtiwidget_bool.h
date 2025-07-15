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

#include "../../qvtiwidget.h"

/**
 * @ingroup    qtwidgetfactory
 * @brief      A checkbox widget.
 */
class QVTIWidget_bool : public QVTIWidget {
  Q_OBJECT

  public:

    QVTIWidget_bool(AbstractValueTypeInterface* valuetypeinterface, QWidget* parent = NULL);
    virtual ~QVTIWidget_bool() = default;

    void synchronizeVTI() override;

  private:
    QCheckBox* getCheckBox() {
      return dynamic_cast<QCheckBox*>(getWidget());
    }

    virtual bool validateValue() override {return true;};
};
