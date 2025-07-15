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

#include <QDebug>
#include <QTimeEdit>

#include "qvtiwidget_time.h"

QVTIWidget_time::QVTIWidget_time(AbstractValueTypeInterface* valueTypeInterface, QWidget* parent)
    : QVTIWidget(valueTypeInterface, new QTimeEdit(parent)) {

  getTimeEdit()->setDisplayFormat(QLocale::system().timeFormat());

  connect(getTimeEdit(), &QTimeEdit::dateTimeChanged, this, [this](const QDateTime &datetime) {
    QVTIWidget::setValue(datetime.toString("HH:mm").toStdString());
  });
}

void QVTIWidget_time::synchronizeVTI() {
  bool oldState = getTimeEdit()->blockSignals(true);
  auto hint = getValueTypeInterface()->getHint();
  if (hint->hasEntry("limit_min")) {
    std::string limitmin;
    hint->getEntry("limit_min", limitmin);
    getTimeEdit()->setMinimumTime(QTime::fromString(limitmin.c_str(), "HH:mm"));
  }
  if (hint->hasEntry("limit_max")) {
    std::string limitmax;
    hint->getEntry("limit_max", limitmax);
    getTimeEdit()->setMaximumTime(QTime::fromString(limitmax.c_str(), "HH:mm"));
  }

  std::string value;
  getValue(value);
  getTimeEdit()->setTime(QTime::fromString(value.c_str(), "HH:mm"));
  qDebug() << "dateTimeChangedx " << QTime::fromString(value.c_str(), "HH:mm") << " " << value.c_str() << " " << getTimeEdit()->dateTime();
  getTimeEdit()->blockSignals(oldState);
}
