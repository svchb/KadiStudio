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
#include <QVBoxLayout>
#include <QDateTimeEdit>
#include <QCalendarWidget>

#include "qvtiwidget_datetime.h"

QVTIWidget_datetime::QVTIWidget_datetime(AbstractValueTypeInterface* valueTypeInterface, QWidget* parent)
    : QVTIWidget(valueTypeInterface, new QDateTimeEdit(parent)) {

  getDateTimeEdit()->setCalendarPopup(true);
  getDateTimeEdit()->setDisplayFormat(QLocale::system().dateTimeFormat());

  connect(getDateTimeEdit(), &QDateTimeEdit::dateTimeChanged, [this](const QDateTime& datetime) {
    QVTIWidget::setValue(datetime.toString(Qt::ISODate).toStdString());
  });
}

void QVTIWidget_datetime::synchronizeVTI() {
  auto hint = getValueTypeInterface()->getHint();
  if (hint->hasEntry("limit_min")) {
    std::string limitmin;
    hint->getEntry("limit_min", limitmin);
    getDateTimeEdit()->setMinimumDateTime(QDateTime::fromString(limitmin.c_str(), Qt::ISODate));
  }
  if (hint->hasEntry("limit_max")) {
    std::string limitmax;
    hint->getEntry("limit_max", limitmax);
    getDateTimeEdit()->setMaximumDateTime(QDateTime::fromString(limitmax.c_str(), Qt::ISODate));
  }

  std::string value;
  getValue(value);
  getDateTimeEdit()->setDateTime(QDateTime::fromString(value.c_str(), Qt::ISODate));
}
