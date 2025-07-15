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

#include <cstdio>

#include <QMessageBox>
#include <QIcon>
#include <QDirIterator>

#include <framework/pluginframework/pluginmanager.h>
#include <framework/pluginframework/plugininfo/plugininfo.h>
#include <framework/commandlineparser/commandlineparser.h>
#include <framework/mainwindow/mainwindow.h>
#include <framework/workspace/workspace.h>
#include <framework/dock/dockwindow.h>

#include "application.h"

void myMessageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg) {
  QByteArray localMsg = msg.toLocal8Bit();
  switch (type) {
  case QtDebugMsg:
      fprintf(stderr, "  (DD) %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
      break;
  case QtInfoMsg:
      fprintf(stderr, "  (II) %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
      break;
  case QtWarningMsg:
      fprintf(stderr, "  (WW) %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
      break;
  case QtCriticalMsg:
      fprintf(stderr, "Critical: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
      break;
  case QtFatalMsg:
      fprintf(stderr, "Fatal: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
      break;
  }
}

Application::Application(const QString& appname, int& argc, char** argv) :
    QApplication(argc, argv) {
  qInstallMessageHandler(myMessageOutput);

  QApplication::setOrganizationName("KIT");
  QApplication::setOrganizationDomain("kit.edu");
  QApplication::setApplicationName(appname);
  QApplication::setWindowIcon(QIcon(":/studio/framework/pixmaps/kadi.png"));

  this->autostart = false;
  pluginmanager = LibFramework::PluginManager::getInstance();
  loadLocalPlugins();
}

Application::~Application() {
}

int Application::evaluateHelp(int argc, char** argv) {
  CommandLineParser parser = CommandLineParser();
  parser.setArguments(argc, argv);
  int ret = parser.evaluateHelp();
  if (ret == -2) {
    autostart = parser.autostartPlugins();
    ret = (autostart == false) ? -1 : 1;
  }
  return ret;
}

void Application::loadLocalPlugins() {
  QDir librarydir(Application::applicationDirPath());
#ifdef Q_OS_WIN
  librarydir.cd("../");
#else
  librarydir.cd("../lib");
#endif
  QString plugindirpath = librarydir.absolutePath();

  QDir pluginsdir(plugindirpath + "/plugins/");

  QDirIterator iterator(pluginsdir.absolutePath(),
#ifdef Q_OS_WIN
                        QStringList("*.dll"),
#else
                        QStringList("*.so"),
#endif
                        QDir::Files,
                        QDirIterator::Subdirectories);
  while (iterator.hasNext()) {
    iterator.next();
    QString filename = iterator.filePath();
    pluginmanager->addPlugin(filename.toStdString());
  }
}

void Application::showMainWindow() {
  MainWindow *mainwindow = MainWindow::getInstance();
  mainwindow->show();
  // show pluginchooser after launch
  // but only when no plugin appended via commandline
  if (!autostart) {
    if (pluginmanager->getPluginInfos("/plugins/application").empty()) {
      QMessageBox::critical(mainwindow, QApplication::applicationName(), tr("No application plugins found. Please check your installation."));
    } else {
      mainwindow->showPluginChooserDialog();
    }
  }
}

void Application::addTab(const QString& callernamespace, QWidget* widget, const QString& name) {
  Workspace *workspace = MainWindow::getInstance()->getWorkspace();
  workspace->addTab(callernamespace, widget, name);
}

void Application::addToolBar(const QString& callernamespace, QToolBar* toolbar) {
  Workspace *workspace = MainWindow::getInstance()->getWorkspace();
  workspace->addToolBar(callernamespace, toolbar);
}

void Application::setTabName(const QString& callernamespace, const QString& text) {
  Workspace *workspace = MainWindow::getInstance()->getWorkspace();
  workspace->setTabName(callernamespace, text);
}

void Application::setActiveTab(const QString &callernamespace) {
  Workspace *workspace = MainWindow::getInstance()->getWorkspace();
  workspace->setActiveTab(callernamespace);
}


bool Application::isTabActive(const QString &callernamespace) {
  Workspace *workspace = MainWindow::getInstance()->getWorkspace();
  return workspace->isTabActive(callernamespace);
}

void Application::addMenu(const QString& callernamespace, QMenu* menu) {
  MainWindow *mainwindow = MainWindow::getInstance();
  mainwindow->addMenu(callernamespace, menu);
}

void Application::showMessage(const QString& orgin, const QString& msg, int level) {
  MainWindow *mainwindow = MainWindow::getInstance();
  mainwindow->showMessage(orgin, msg, level);
}

void Application::clearMessage() {
  MainWindow *mainwindow = MainWindow::getInstance();
  mainwindow->clearMessage();
}
