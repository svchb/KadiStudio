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

#include <QLineEdit>

#include "../../qvtiwidget.h"

/**
 * @brief      A lineedit widget with a property.
 * @ingroup    qtwidgetfactory
 */
class QVTIWidget_string : public QVTIWidget {
  Q_OBJECT

  public:
    QVTIWidget_string(AbstractValueTypeInterface* value_type_interface, QWidget* parent = nullptr);
    virtual ~QVTIWidget_string() = default;

    void synchronizeVTI() override;

  private:
    virtual bool validateValue() override {return true;/*TODO*/};

    QLineEdit* getLineEdit() {
      return dynamic_cast<QLineEdit*>(getWidget());
    }

  private Q_SLOTS:
    void setValue(const QString& value);
    void checkLength();

};
