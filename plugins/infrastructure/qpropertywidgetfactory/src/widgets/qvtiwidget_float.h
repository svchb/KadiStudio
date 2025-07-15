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

#if 0 // Idea use in the future
/** from https://stackoverflow.com/questions/54295639/how-to-set-qdoublespinbox-to-scientific-notation
  */
class SciNotDoubleSpinbox : public QDoubleSpinBox {
  Q_OBJECT

  public:
    SciNotDoubleSpinbox(QWidget* parent = 0) : QDoubleSpinBox(parent) {}
    virtual ~SciNotDoubleSpinbox() {}

    // Change the way we read the user input
    double valueFromText(const QString& text) const {
      return text.toDouble();
    }

    // Change the way we show the internal number
    QString textFromValue(double value) const {
      return QString::number(value, 'E', 6);
    }

    // Change the way we validate user input (if validate => valueFromText)
    QValidator::State validate(QString& text, int&) const {

      // Try to convert the string to double
      bool ok;
      text.toDouble(&ok);
      // See if it's a valid Double
      QValidator::State validationState;
      if (ok) {
        // If string conversion was valid, set as ascceptable
        validationState = QValidator::Acceptable;

      } else {
        // If string conversion was invalid, set as invalid
        validationState = QValidator::Invalid;
      }
      return validationState;
    }

};
#endif

/**
 * @brief      A spinbox widget for float values with a property.
 * @ingroup    qtwidgetfactory
 */
class QVTIWidget_float : public QVTIWidget {
  Q_OBJECT

  public:

    explicit QVTIWidget_float(AbstractValueTypeInterface* valuetypeinterface, QWidget* parent = nullptr);
    ~QVTIWidget_float() override = default;

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
