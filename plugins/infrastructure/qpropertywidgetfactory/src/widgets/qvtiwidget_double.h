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

#include "../../qvtiwidget.h"

class QLineEdit;


/**
 * @brief      A spinbox widget for double values with a property.
 * @ingroup    qtwidgetfactory
 */
class QVTIWidget_double : public QVTIWidget {
  Q_OBJECT

  public:

    explicit QVTIWidget_double(AbstractValueTypeInterface* valuetypeinterface, QWidget* parent = nullptr);
    ~QVTIWidget_double() override = default;

    void synchronizeVTI() override;

  public Q_SLOTS:
    void eTriggered();
    void checkE();

  private Q_SLOTS:
    void checkLength();

  private:
    bool validateValue() override;

    QLineEdit* getLineEdit() const {
      return lineedit;
    }
    QLineEdit *lineedit;
    QPushButton *ebutton;

};
