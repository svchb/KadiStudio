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
#include <QDialog>
#include <QVBoxLayout>

#include <pluginframework/pluginmanager.h>
#include <plugins/infrastructure/dialogs/tooldialog/widgetvalueinterface.h>

#include "test_tooldialog_choosetool.h"

TestToolDialogChooseTool::TestToolDialogChooseTool() {
  initTestCase();
}

TestToolDialogChooseTool::~TestToolDialogChooseTool() {
  cleanupTestCase();
}

void TestToolDialogChooseTool::initTestCase() {
  QSettings settings;
  QString fileName = settings.fileName();

  QFile file(fileName);
  file.open(QIODevice::ReadOnly);
  _settingsBackup = file.readAll();
  file.close();
}

void TestToolDialogChooseTool::cleanupTestCase() {
  QSettings settings;
  QString fileName = settings.fileName();

  QFile file(fileName);
  file.open(QIODevice::WriteOnly);
  file.write(_settingsBackup);
  file.flush();
  file.close();
}

void TestToolDialogChooseTool::executeTestCase() {
  LibFramework::PluginManager pluginmanager;
  pluginmanager.load("tooldialog");
  WidgetValueInterface *plugin = pluginmanager.getInterface<WidgetValueInterface*>("/infrastructure/widget/tooldialog");

  QDialog *dialog = new QDialog;
  QVBoxLayout *layout = new QVBoxLayout;
  dialog->setLayout(layout);
  QString toolIdentificationStringWithFiles = "/usr/bin/domainshift '/path/to/nowhere.phase' '/path/to/nowhere.phase' -z 30";
  ToolWidget *toolwidget = plugin->getToolWidget(dialog, toolIdentificationStringWithFiles, true);

  QMap<QString, QVariant> tooldata;
  toolwidget->startTool(&tooldata);

  dialog->close();
  delete dialog;

  QString toolName = "domainshift";
  QString toolExecutable = "/usr/bin/domainshift";
  QStringList toolArguments;
  toolArguments << "-z" << "30";
  QString toolIdentificationString = "/usr/bin/domainshift -z 30";

  QString toolNameResult = tooldata.value("toolname").toString();
  QString toolExecutableResult = tooldata.value("toolexecutable").toString();
  QStringList toolArgumentsResult = tooldata.value("toolarguments").toStringList();
  QString toolIdentificationStringResult = tooldata.value("toolstring").toString();
  int inPortCountResult = tooldata.value("infilescount").toInt();
  int outPortCountResult = tooldata.value("outfilescount").toInt();

  QCOMPARE(toolNameResult, toolName);
  QCOMPARE(toolExecutableResult, toolExecutable);
  QCOMPARE(toolArgumentsResult, toolArguments);
  QCOMPARE(toolIdentificationStringResult, toolIdentificationString);
  QCOMPARE(inPortCountResult, 0);
  QCOMPARE(outPortCountResult, 0);
}
