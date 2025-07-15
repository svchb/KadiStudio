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

#include <QLocale>
#include <QMessageBox>
#include <QHBoxLayout>

#include <framework/enhanced/qlineedit_withunitlabel.h>

#include "qvtiwidget_double.h"

/** from https://stackoverflow.com/questions/12459059/qstringnumber-f-format-without-trailing-zeros
 */
static QString mynumber(qreal p_number, char p_format, int p_precision) {
  if (p_format=='h') {
    // like 'f' but removes trailing zeros and the '.' evtl
    QString res = QString::number(p_number, 'g', p_precision);
    bool hasE = res.contains('e', Qt::CaseInsensitive);
    if (not hasE) return res;
    res = QString::number(p_number, 'f', p_precision);
    int countTrailingToRemove=0;
    for (QString::const_reverse_iterator it=res.crbegin(); it!=res.crend(); ++it) {
      if (*it=='0') {
        countTrailingToRemove++;
      } else {
        if (*it=='.') {
          countTrailingToRemove++;
        }
        break;
      }
    }

    return res.mid(0, res.length()-countTrailingToRemove);
  }
  return QString::number(p_number, p_format, p_precision);
}

QVTIWidget_double::QVTIWidget_double(AbstractValueTypeInterface* valuetypeinterface, QWidget* parent)
    : QVTIWidget(valuetypeinterface, new QWidget(parent)) {

  getWidget()->setLayout(new QHBoxLayout());
  getWidget()->layout()->setContentsMargins(0, 0, 0, 0);

  QString unitstring;
  if (valuetypeinterface->getHint()->hasEntry("unit")) {
    unitstring = QString::fromStdString(valuetypeinterface->getHint()->getEntry("unit"));
  }
  lineedit = new QLineEdit_withUnitLabel(unitstring, getWidget());
  getLineEdit()->setAutoFillBackground(true);

  getLineEdit()->installEventFilter(this->filter);
  getWidget()->layout()->addWidget(getLineEdit());

  this->ebutton = new QPushButton("e", getWidget());
  this->ebutton->installEventFilter(this->filter);
  this->ebutton->setToolTip(tr("switches view\nWarning: if you enter too many decimals then precision may be lost"));
  this->ebutton->setFixedWidth(20);
  getWidget()->layout()->addWidget(this->ebutton);

  connect(this->ebutton, &QPushButton::clicked,               this,   &QVTIWidget_double::eTriggered);
  connect(getLineEdit(), &QLineEdit::textChanged,             this,   &QVTIWidget_double::needsValidation);
  connect(this,          &QVTIWidget_double::needsValidation, this,   &QVTIWidget_double::validate);
  connect(this,          &QVTIWidget_double::inputValidated,  this,   &QVTIWidget_double::highlightValidateState);

  connect(getLineEdit(), &QLineEdit::textEdited,              this,   &QVTIWidget_double::checkLength);

  connect(this, &QVTIWidget::inputValidated, this, [&](bool isvalid) {
    if (/*validateValue()*/isvalid) {
      double new_value = getLineEdit()->text().toDouble();
      setValue(new_value);
    }
  });
}

void QVTIWidget_double::synchronizeVTI() {
  double value;
  getValue(value);
  checkE();
  bool isE = getLineEdit()->text().contains('e', Qt::CaseInsensitive);

  QString buffer;
  if (isE) {
    buffer = QString::number(value, 'e', 10);
  } else {
    buffer = mynumber(value, 'h', 10);
  }
  getLineEdit()->setText(buffer);
}

void QVTIWidget_double::eTriggered() {
  if (!validateValue()) {
    QMessageBox::warning(getWidget(), tr("Kadi Studio"),
                                      tr("Input value does not appear to be a floating point number. Please correct it."));
    return;
  }

  bool isE = getLineEdit()->text().contains('e', Qt::CaseInsensitive);

  QString buffer;
  if (isE) {
    buffer = mynumber(getLineEdit()->text().toDouble(), 'h', 10);
  } else {
    buffer = QString::number(getLineEdit()->text().toDouble(), 'e', 10);
  }
  getLineEdit()->setText(buffer);
}

void QVTIWidget_double::checkE() {
  if (getLineEdit()->text().contains('e', Qt::CaseInsensitive)) {
    ebutton->setText("f");
  } else {
    ebutton->setText("e");
  }
}

bool QVTIWidget_double::validateValue() {
  checkLength();

  bool ok = false;
  double value = getLineEdit()->text().toDouble(&ok);

  if (!ok) return false;

  auto hint = getValueTypeInterface()->getHint();
  double limitmin = std::numeric_limits<double>::lowest();
  if (hint->hasEntry("limit_min")) {
    hint->getEntry("limit_min", limitmin);
  }
  double limitmax = std::numeric_limits<double>::max();
  if (hint->hasEntry("limit_max")) {
    hint->getEntry("limit_max", limitmax);
  }

  return (value >= limitmin) && (value <= limitmax);
}

void QVTIWidget_double::checkLength() {
  if (getLineEdit()->text().size() >= getLineEdit()->maxLength()) {
    QMessageBox::warning(getWidget(), tr("Kadi Studio"),
                                      tr("Value was truncated (exceeded maximum length)."));
  }
}
