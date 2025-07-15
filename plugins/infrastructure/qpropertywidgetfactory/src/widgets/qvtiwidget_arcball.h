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

#include <QGroupBox>
#include "../../qvtiwidget.h"

class QDoubleSpinBox;

/**
 * @brief      An arcball replacement property.
 * @ingroup    qtwidgetfactory
 */
class QVTIWidget_arcball : public QVTIWidget {
  Q_OBJECT

  public:

    QVTIWidget_arcball(AbstractValueTypeInterface* valuetypeinterface, QWidget* parent = NULL);
    virtual ~QVTIWidget_arcball() = default;

    void synchronizeVTI() override;

    void setLimits(double min, double max);

  private Q_SLOTS:
    void setValue(int index, double value);
    void itemValueInternal(double value);

  Q_SIGNALS:
    void itemValueChanged(int index, double value);

  private:
    QDoubleSpinBox *xoffsetdoublespinbox;
    QDoubleSpinBox *yoffsetdoublespinbox;
    QDoubleSpinBox *zoffsetdoublespinbox;

    double rotation[3];

};
