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

#include <QAction>
#include <QEvent>
#include <QFocusEvent>

#include "qlineeditclearable.h"

QLineEditClearable::QLineEditClearable(QWidget *parent)
    : QLineEdit(parent) {

  setClearButtonEnabled(true);

  QList<QAction*> actionlist = this->findChildren<QAction*>();

  connect(actionlist.first(), &QAction::triggered, this, [this]() {
    clear();
    emit cleared();
  });

  firsttimemousefocusin = true;
}

void QLineEditClearable::focusInEvent(QFocusEvent *e) {
  if (e->reason() == Qt::MouseFocusReason) {
    firsttimemousefocusin = true;
  } else if (e->reason() == Qt::OtherFocusReason) {
    this->selectAll();
  }
  QLineEdit::focusInEvent(e);
}

void QLineEditClearable::mousePressEvent(QMouseEvent *e) {
  if (firsttimemousefocusin) {
    this->selectAll();
    firsttimemousefocusin = false;
  } else {
    QLineEdit::mousePressEvent(e);
  }
}
