/* Copyright 2022 Karlsruhe Institute of Technology
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

#include <vector>

#include <QStatusBar>
#include <QMenuBar>
#include <QSettings>
#include <QCloseEvent>
#include <QAction>
#include <QFile>
#include <QDateTime>
#include <QDockWidget>
#include <QDebug>
#include <QApplication>

#include <workspace/workspace.h>
#include <dock/dockinterface.h>
#include <statusbar/statusbarinterface.h>
#include <pluginframework/pluginchooser/pluginchooser.h>
#include <pluginframework/plugininfo/plugininfo.h>
#include <pluginframework/pluginmanager.h>

#include "aboutdialog.h"
#include "helpdialog.h"
#include "mainwindow.h"

const QString sGeometry = QString::fromLocal8Bit("/geometry");
const QString sWidth = QString::fromLocal8Bit("/size/width");
const QString sHeight = QString::fromLocal8Bit("/size/height");
const QString sPosX = QString::fromLocal8Bit("/position/x");
const QString sPosY = QString::fromLocal8Bit("/position/y");

template MainWindow* Singleton<MainWindow>::getInstance();

MainWindow::MainWindow() : QMainWindow(), signalmapper(this) {
  this->pluginmanager = LibFramework::PluginManager::getInstance();
  this->helpmenuaction = nullptr;

  QMainWindow::setWindowTitle(tr("Kadi Studio"));

  workspace = new Workspace();
  setCentralWidget(workspace->getWidget());

  readSettings();
  createMenus();
}

MainWindow::~MainWindow() {
  for (const QString& callernamespace : pluginmenus.keys()) {
    removeMenu(callernamespace);
  }
  delete dockwindowsmenu;
  delete workspace;
}

void MainWindow::createMenus() {
  // create standard actions
  QAction *exitaction = new QAction(tr("&Exit"), this);
  exitaction->setShortcut(tr("Ctrl+Q"));

  // create other actions
  QAction *aboutaction_kadi = new QAction(QIcon(":/studio/framework/pixmaps/kadi.png"), tr("&About") + " " +  windowTitle(), this);
  QAction *aboutaction_qt = new QAction(this->style()->standardIcon(QStyle::SP_TitleBarMenuButton), tr("&About Qt"), this);

  nodockwindowsaction = new QAction("&No dockwindows available", this);
  nodockwindowsaction->setDisabled(true);

  QAction *helpaction = new QAction(QIcon(":/studio/framework/pixmaps/info.png"), tr("&Help"), this);
  helpaction->setStatusTip(tr("Opens the online-manual"));
  helpaction->setShortcut(tr("F1"));

  QAction *pluginchooseraction = new QAction(QIcon(":/studio/framework/pixmaps/plugins.png"), tr("&Choose Plugin..."), this);
  pluginchooseraction->setStatusTip(tr("Opens the plugin chooser dialog"));
  pluginchooseraction->setShortcut(tr("Ctrl+P"));

  // connect Actions with triggers
  connect(aboutaction_kadi, SIGNAL(triggered()), this, SLOT(showAboutDialog()));
  connect(aboutaction_qt, SIGNAL(triggered()), qApp, SLOT(aboutQt()));
  connect(helpaction, SIGNAL(triggered()), this, SLOT(showHelpDialog()));
  connect(exitaction, SIGNAL(triggered()), this, SLOT(close()));
  connect(pluginchooseraction, SIGNAL(triggered()), this, SLOT(showPluginChooserDialog()));

  // create Menu
  pluginmenu = menuBar()->addMenu(tr("&Studio"));

  // dummy action for header only
  QAction *availablePluginsAction = new QAction(QIcon(":/studio/framework/pixmaps/plugins.png"), tr("No Plugins available"), this);
  pluginmenu->addAction(availablePluginsAction);

  std::vector<const LibFramework::PluginInfo*> plugininfos = pluginmanager->getPluginInfos("/plugins/application");

  // check if there are application plugins
  if (!plugininfos.empty()) {
    availablePluginsAction->setText(tr("Available Plugins"));
    pluginmenu->addSeparator();
    // list all
    for (const LibFramework::PluginInfo *plugininfo : plugininfos) {
      QString namespacepath = QString::fromStdString(plugininfo->getNamespace());
      QString name = namespacepath.split("/").last();
      QAction *pluginaction = new QAction(name, this);
      connect(pluginaction, SIGNAL(triggered()), &signalmapper, SLOT(map()));
      signalmapper.setMapping(pluginaction, namespacepath);
      pluginmenu->addAction(pluginaction);
    }
  } else {
    availablePluginsAction->setDisabled(true);
    pluginchooseraction->setText("Please check your installation");
    pluginchooseraction->setDisabled(true);
  }

  connect(&signalmapper, SIGNAL(mapped(QString)), this, SLOT(startPlugin(QString)));

  pluginmenu->addSeparator();
  pluginmenu->addAction(pluginchooseraction);
  pluginmenu->addSeparator();

  connect(pluginmenu, SIGNAL(aboutToShow()), this, SLOT(evaluatePluginStatus()));

  pluginmenu->addAction(exitaction);

  dockwindowsmenu = menuBar()->addMenu(tr("&Dock Windows"));
  dockwindowsmenu->addAction(nodockwindowsaction);

  // this enables to have a menu on the right side of the menu bar
  QMenuBar *cornerMenuBar = new QMenuBar(menuBar());
  QMenu *helpmenu = new QMenu(tr("&Help"), cornerMenuBar);
  helpmenu->addAction(helpaction);
  helpmenu->addSeparator();
  helpmenu->addAction(aboutaction_kadi);
  helpmenu->addAction(aboutaction_qt);
  cornerMenuBar->addMenu(helpmenu);
  menuBar()->setCornerWidget(cornerMenuBar);
}

void MainWindow::evaluatePluginStatus() {
  std::vector<const LibFramework::PluginInfo*> plugininfos = pluginmanager->getPluginInfos("/plugins/application");

  for (const LibFramework::PluginInfo *plugininfo : plugininfos) {
    std::string pluginnamespace = plugininfo->getNamespace();
    QString namespacepath = QString::fromStdString(pluginnamespace);
    bool started = pluginmanager->isRunning(pluginnamespace);

    for (QAction *menuaction : pluginmenu->actions()) {
      QString actionname = menuaction->text();
      if (actionname.isEmpty()) {
        continue;
      }

      if (namespacepath.contains(actionname)) {
        if (started) {
          menuaction->setDisabled(true);
        } else {
          menuaction->setEnabled(true);
        }
        break;
      }
    }
  }
}

void MainWindow::show() {
  QMainWindow::show();
  QMainWindow::statusBar()->show();
}

void MainWindow::startPlugin(const QString& namespacepath) {
  pluginmanager->run(namespacepath.toStdString());
}

void MainWindow::readSettings() {
  QSettings settings(qApp->applicationName(), "main");

  settings.beginGroup(sGeometry);
  {
    int width = settings.value(sWidth, 1024).toInt();
    int height = settings.value(sHeight, 768).toInt();
    if (width > 100 && height > 100) {
      QMainWindow::resize(width, height);
    }

    int x = settings.value(sPosX, 100).toInt();
    int y = settings.value(sPosY, 100).toInt();

    QMainWindow::move(x, y);
  }
  settings.endGroup();

  logfilename = settings.value("logfilename", qApp->applicationName() + ".log").toString();
}

void MainWindow::writeSettings() {
  // save the settings at the end of a session
  QSettings settings(qApp->applicationName(), "main");

  settings.beginGroup(sGeometry);
  {
    QSize size = QMainWindow::size();
    QPoint position = QMainWindow::pos();

    settings.setValue(sWidth, size.width());
    settings.setValue(sHeight, size.height());
    settings.setValue(sPosX, position.x());
    settings.setValue(sPosY, position.y());
  }
  settings.endGroup();

  settings.setValue("logfilename", logfilename);
}

void MainWindow::closeEvent(QCloseEvent* event) {
  std::vector<std::string> pluginlist = pluginmanager->getRunningNamespaces();
  bool successclose = true;

  for (std::vector<std::string>::const_iterator it = pluginlist.begin(); it != pluginlist.end(); ++it) {
    QWidget *widget = this->findChild<QWidget*>(QString(it->c_str()));
    if (widget) {
      successclose = widget->close();
    }
  }

  if (successclose) {
    // store gui settings
    this->writeSettings();
    // accept the close
    event->accept();
  } else {
    event->ignore();
  }
}

void MainWindow::showPluginChooserDialog() {
  PluginChooser pluginchooser(this);
  pluginchooser.show("/plugins/application", true);
  for (const QString &pluginnamespace : pluginchooser.getSelection()) {
    auto plnamespace = pluginnamespace.toStdString();
    pluginmanager->toggle(plnamespace);
    if (pluginmanager->isUnloaded(plnamespace)) {
      removePluginItems(pluginnamespace);
    }
  }
}

void MainWindow::showAboutDialog() {
  AboutStudioDialog dialog(workspace->getActiveNamespace(), this);
  dialog.exec();
}

void MainWindow::showHelpDialog() {
  HelpDialog dialog(workspace->getActiveNamespace(), this);
  dialog.exec();
}

/** @brief Shows a Statusmessage in the Statusbar and logs the messages in a logfile depending on the level selected.
 *
 *  @param origin Origination File the message is produced from
 *  @param msg Message that should be shown
 *  @param level Level of logging (see enumeration)
 */
void MainWindow::showMessage(const QString& origin, const QString& msg, int level) {
  QMainWindow::statusBar()->showMessage(msg, 5000);

  qDebug() << origin << ": " << msg;

  if (level == MSG_NOLOG || level == MSG_LOG || level == MSG_DEBUG_NOLOG || level == MSG_DEBUG_LOG) {
    QFile logfile(logfilename);
    logfile.open(QIODevice::ReadWrite | QIODevice::Text | QIODevice::Append);
    QDateTime dateTime = QDateTime();

    logfile.write(dateTime.toString("dd.MM.yyyy hh:mm:ss").toLatin1() + " : " +
                  origin.toLatin1() + " : " +
                  msg.toLatin1() + "\n");

    logfile.flush();
    logfile.close();
  }
}

void MainWindow::clearMessage() {
  statusBar()->clearMessage();
}

void MainWindow::showPluginDockWidgets(const QString& callernamespace) {
  QList<QMap<QDockWidget*, Qt::DockWidgetArea> > dockwidgetlist = plugindockwidgets.values();
  for (int i = 0; i < dockwidgetlist.length(); ++i) {
    QMap<QDockWidget*, Qt::DockWidgetArea> dockwidgets = plugindockwidgets.values().at(i);
    for (QDockWidget *dockwidget : dockwidgets.keys()) {
      dockwidget->hide();
    }
  }
  for (QDockWidget *dockwidget : plugindockwidgets.value(callernamespace).keys()) {
    dockwidget->show();
  }
}

void MainWindow::dockWindow(const QString& callernamespace, DockWindow* dockwindow, bool dock) {
  for (QDockWidget *dockwidget : plugindockwidgets.value(callernamespace).keys()) {
    if (dockwidget == dockwindow) {
      dockwindow->setFloating(!dock);
    }
  }
}

void MainWindow::addDockWindow(const QString& callernamespace, DockWindow* dockwindow) {
  QMap<QDockWidget*, Qt::DockWidgetArea> dockwidgets;

  QDockWidget *dockwidget = dockwindow->widget();
  Qt::DockWidgetArea area = dockwindow->area();

  dockwidgets.insert(dockwidget, area);
  addPluginDockWidgets(callernamespace, dockwidgets);
}

void MainWindow::addPluginDockWidgets(const QString& callernamespace, const QMap<QDockWidget*, Qt::DockWidgetArea>& dockwidgets) {
  if (nodockwindowsaction->isVisible()) {
    nodockwindowsaction->setVisible(false);
  }
  // TODO can be rewritten in qt 6.4 for (auto [key, value] : dockwidgets.asKeyValueRange()) {
  for (auto dockwidgetarea = dockwidgets.keyValueBegin(); dockwidgetarea != dockwidgets.keyValueEnd(); ++dockwidgetarea) {
    QDockWidget *dockwidget = dockwidgetarea->first;
    Qt::DockWidgetArea area = dockwidgetarea->second;
    QMainWindow::addDockWidget(area, dockwidget);
    QAction *dockwindowaction = dockwidget->toggleViewAction();
    dockwindowsmenu->addAction(dockwindowaction);
  }
  plugindockwidgets.insert(callernamespace, dockwidgets);
}

void MainWindow::removePluginDockWidgets(const QString& callernamespace) {
  for (QDockWidget *dockwidget : plugindockwidgets.value(callernamespace).keys()) {
    QAction *dockwindowaction = dockwidget->toggleViewAction();
    dockwindowsmenu->removeAction(dockwindowaction);
    QObject *child = dockwidget->widget();
    delete child;
    QMainWindow::removeDockWidget(dockwidget);
  }
  if (!nodockwindowsaction->isVisible() && dockwindowsmenu->isEmpty()) {
    nodockwindowsaction->setVisible(true);
  }
}

void MainWindow::addMenu(const QString& callernamespace, QMenu* menu) {
  if (menuBar()->actions().last() == helpmenuaction) {
    QMenu *menuseparator = new QMenu(tr("&|"));
    menuseparator->setDisabled(true);
    menuBar()->addMenu(menuseparator);
  } else if (!pluginmenus.contains(callernamespace)) {
    for (QMenu *menu : pluginmenus.values()) {
      QAction *menuaction = menu->menuAction();
      menuBar()->removeAction(menuaction);
    }
  }
  menuBar()->addMenu(menu);
  pluginmenus.insert(callernamespace, menu);
}

void MainWindow::removeMenu(const QString& callernamespace) {
  for (QMenu *menu : pluginmenus.values(callernamespace)) {
    QAction *menuaction = menu->menuAction();
    menuBar()->removeAction(menuaction);
  }
  pluginmenus.remove(callernamespace);
  if (menuBar()->actions().last()->menu()->title() == "&|") {
    menuBar()->removeAction(menuBar()->actions().last());
  }
}

void MainWindow::showMenu(const QString& callernamespace) {
  for (QMenu *menu : pluginmenus.values()) {
    QAction *menuaction = menu->menuAction();
    menuBar()->removeAction(menuaction);
  }

  if (!pluginmenus.contains(callernamespace) &&
      menuBar()->actions().last()->text() == "&|") {
    menuBar()->removeAction(menuBar()->actions().last());
    return;
  } else if (pluginmenus.contains(callernamespace) &&
             menuBar()->actions().last() == helpmenuaction) {
    QMenu *menuseparator = new QMenu(tr("&|"));
    menuseparator->setDisabled(true);
    menuBar()->addMenu(menuseparator);
  }

  for (int i = pluginmenus.values(callernamespace).size() - 1; i >= 0; --i) {
    QMenu *menu = pluginmenus.values(callernamespace).at(i);
    menuBar()->addMenu(menu);
  }
}

void MainWindow::showPluginItems(const QString& callernamespace) {
  showMenu(callernamespace);
  showPluginDockWidgets(callernamespace);
}

void MainWindow::removePluginItems(const QString& callernamespace) {
  if (callernamespace.isEmpty()) {
    return;
  }

  removeMenu(callernamespace);
  removePluginDockWidgets(callernamespace);

  workspace->removeTab(callernamespace);
}
