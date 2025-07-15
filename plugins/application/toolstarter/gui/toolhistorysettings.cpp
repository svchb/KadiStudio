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

#include <QApplication>
#include <QSettings>
#include <QDateTime>

#include "toolhistorysettings.h"

ToolHistorySettings::ToolHistorySettings() {
  settings = new QSettings(qApp->applicationName(), "/plugins/application/toolstarter");
  maxentrycount = settings->value("maxentrycount", 15).toInt();
  if (not settings->contains("maxentrycount")) {
    settings->setValue("maxentrycount", maxentrycount);
  }
}

ToolHistorySettings::~ToolHistorySettings() {
  delete settings;
}

void ToolHistorySettings::save(const QString &toolidentificationstring, const QDateTime &datetime) {
  // shift history one up
  settings->beginGroup("recentlyused");
  for (int i = maxentrycount - 2; i >= 0; i--) {
    QString value = settings->value(QString::number(i), "").toString();
    settings->setValue(QString::number(i+1), value);
  }

  settings->setValue("0", toolidentificationstring + ";" + datetime.toString(Qt::ISODate));
  settings->endGroup();
}

QStringList ToolHistorySettings::load() {
  QStringList recentsettings;

  settings->beginGroup("recentlyused");
  for (int i = maxentrycount - 1; i >= 0; i--) {
    recentsettings << settings->value(QString::number(i), "").toString();
  }
  settings->endGroup();

  return recentsettings;
}
