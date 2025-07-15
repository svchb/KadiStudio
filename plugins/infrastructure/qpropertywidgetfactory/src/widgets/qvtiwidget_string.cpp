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

#include <QMessageBox>
#include <framework/enhanced/qlineeditclearable.h>

#include "qvtiwidget_string.h"

QVTIWidget_string::QVTIWidget_string(AbstractValueTypeInterface* valuetypeinterface, QWidget* parent)
    : QVTIWidget(valuetypeinterface, new QLineEditClearable(parent)) {

  connect(getLineEdit(), &QLineEdit::textEdited,  this, &QVTIWidget_string::checkLength);
  // TODO needsValidation plus RegEx

  connect(getLineEdit(), &QLineEdit::textChanged, this, &QVTIWidget_string::setValue);
}

void QVTIWidget_string::synchronizeVTI() {
  std::string value = getValueTypeInterface()->toString(); // use toString because in case of a fall back this could be any value type
  // bool oldState = getLineEdit()->blockSignals(true);
  int cursor_position = getLineEdit()->cursorPosition();
  getLineEdit()->setText(QString::fromStdString(value));
  getLineEdit()->setCursorPosition(cursor_position);
  // getLineEdit()->blockSignals(oldState);
}

void QVTIWidget_string::setValue(const QString& value) {
  std::string actualvalue = getValueTypeInterface()->toString();
  std::string newvalue = value.toStdString();
  if (actualvalue == newvalue) return;
  getValueTypeInterface()->fromString(newvalue); // use fromString because in case of a fall back this could be any value type
}

void QVTIWidget_string::checkLength() {
  if (getLineEdit()->text().size() >= getLineEdit()->maxLength()) {
    QMessageBox::warning(getWidget(), tr("Kadi Studio"),
                                      tr("Text is too long for line edit and was truncated."));
  }
}
