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

#include "qvtiwidget_matrix_lineedit.h"


QVTIWidget_Matrix_lineedit::QVTIWidget_Matrix_lineedit(AbstractValueTypeInterface* valuetypeinterface, QWidget* parent)
    : QVTIWidget(valuetypeinterface, new QLineEdit(parent)) {

  connect(getLineEdit(), SIGNAL(textChanged(QString)),     this,   SIGNAL(needsValidation()));
  connect(getLineEdit(), SIGNAL(textEdited(QString)),      this,   SLOT(updateValue(QString)));
}

void QVTIWidget_Matrix_lineedit::synchronizeVTI() {
  std::string value = getValueTypeInterface()->toString();
  bool oldState = getLineEdit()->blockSignals(true);
  int cursor_position = getLineEdit()->cursorPosition();
  getLineEdit()->setText(QString::fromStdString(value));
  getLineEdit()->setCursorPosition(cursor_position);
  getLineEdit()->blockSignals(oldState);
}

bool QVTIWidget_Matrix_lineedit::validateValue() {
  // TODO get Regex from keytype when right Dimension is accessible;
  if (validateSize()) {
#if 0
    char primitivetype = Keytype_getPrimitiveChar(&keytype);
    QRegExp exp;
    if (primitivetype == 'i') {
      exp = QRegExp("^\\[(\\((\\d+(,\\d+)*)?\\)(,\\((\\d+(,\\d+)*)?\\))*)?\\]$");
    } else if (primitivetype == 'f') {
      exp = QRegExp("^\\[((\\((\\d+(\\.\\d+)?(e[+|-]?((30[0-8]|[1-2]\\d\\d)|(\\d\\d?)))?(,\\d+(\\.\\d+)?(e[+|-]?((30[0-8]|[1-2]\\d\\d)|(\\d\\d?)))?)*)?\\))(,\\((\\d+(\\.\\d+)?(e[+|-]?((30[0-8]|[1-2]\\d\\d)|(\\d\\d?)))?(,\\d+(\\.\\d+)?(e[+|-]?((30[0-8]|[1-2]\\d\\d)|(\\d\\d?)))?)*)?\\))*)?\\]$");
    } else if (primitivetype == 'b') {
      exp = QRegExp("^\\[((\\(([01](,[01])*)?\\))(,\\(([01](,[01])*)?\\))*)?\\]$");
    } else { // test for valid matrix
      exp = QRegExp("^\\[((\\(.*\\))(,\\(.*\\))*)?\\]$");
    }
    return exp.exactMatch(matrixline->text());
#else
    return true;
#endif
  } else {
    return false;
  }
}

void QVTIWidget_Matrix_lineedit::updateValue(const QString& value) {
  if (validateSize() && validateValue()) {
    getValueTypeInterface()->fromString(value.toStdString());
  }
}

bool QVTIWidget_Matrix_lineedit::validateSize() {
  if (getValueTypeInterface()->toString().size() < getLineEdit()->maxLength()) {
    return true;
  } else {
    return false;
  }
}
