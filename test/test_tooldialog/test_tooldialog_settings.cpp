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

#include <QtTest/QTest>
#include <QFile>
#include <QSettings>

#include "test_tooldialog_settings.h"

TestToolDialogSettings::TestToolDialogSettings() {
  initTestCase();
}

TestToolDialogSettings::~TestToolDialogSettings() {
  cleanupTestCase();
}

void TestToolDialogSettings::initTestCase() {
  QSettings settings;
  QString fileName = settings.fileName();

  QFile file(fileName);
  file.open(QIODevice::ReadOnly);
  _settingsBackup = file.readAll();
  file.close();

  _settings = new ToolHistorySettings;
}

void TestToolDialogSettings::cleanupTestCase() {
  QSettings settings;
  QString fileName = settings.fileName();

  QFile file(fileName);
  file.open(QIODevice::WriteOnly);
  file.write(_settingsBackup);
  file.flush();
  file.close();

  delete _settings;
}

void TestToolDialogSettings::executeTestCase() {
  saveSettings();
  loadSettings();
}

void TestToolDialogSettings::saveSettings() {
  QString toolIdentificationString = "/usr/bin/domainshift '/path/to/nowhere.phase' '/path/to/nowhere.phase' -z 30";
  _settings->save(toolIdentificationString);
}

void TestToolDialogSettings::loadSettings() {
  QStringList recentTools = _settings->load();
  QStringList entires = recentTools.filter("/usr/bin/domainshift '/path/to/nowhere.phase' '/path/to/nowhere.phase' -z 30");
  QCOMPARE(entires.isEmpty(), false);
}
