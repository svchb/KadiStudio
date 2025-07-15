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

#pragma once

#include <QMainWindow>
#include <QMultiMap>
#include <QSignalMapper>

#include <framework/dock/dockwindow.h>
#include <framework/pluginframework/pluginmanagerinterface.h>

#include <cpputils/singleton.hpp>

class Workspace;


/**
 * @brief      The mainwindow widget.
 * @ingroup    framework
 */
class DLLAPI MainWindow : public QMainWindow, public Singleton<MainWindow> {
    Q_OBJECT

  friend class Singleton<MainWindow>;

  public:
    virtual ~MainWindow();

    void showPluginItems(const QString& callernamespace);
    void removePluginItems(const QString& callernamespace);

    void showMessage(const QString& orgin, const QString& msg, int level);
    void clearMessage();

    Q_INVOKABLE void addDockWindow(const QString& callernamespace, DockWindow* dockwindow);
    Q_INVOKABLE void dockWindow(const QString& callernamespace, DockWindow* dockwindow, bool dock);
    void addMenu(const QString& callernamespace, QMenu* menu);

    Workspace* getWorkspace() {
      return workspace;
    }

    void show();

  public Q_SLOTS:
    void showPluginChooserDialog();

  protected:
    void closeEvent(QCloseEvent* event) override;

  private Q_SLOTS:
    void showAboutDialog();
    void showHelpDialog();
    void evaluatePluginStatus();
    void startPlugin(const QString& namespacepath);

  private:
    MainWindow();

    void readSettings();
    void writeSettings();
    void createMenus();
    void showMenu(const QString& callernamespace);
    void removeMenu(const QString& callernamespace);
    void removePluginDockWidgets(const QString& callernamespace);
    void showPluginDockWidgets(const QString& callernamespace);
    void addPluginDockWidgets(const QString& callernamespace, const QMap<QDockWidget*, Qt::DockWidgetArea>& dockwidgets);

    LibFramework::PluginManagerInterface *pluginmanager;
    QAction *nodockwindowsaction;
    QAction *helpmenuaction;
    QMenu *pluginmenu;
    QMenu *dockwindowsmenu;
    QSignalMapper signalmapper;
    QMultiMap<QString, QMap<QDockWidget*, Qt::DockWidgetArea> > plugindockwidgets;
    QMultiMap<QString, QMenu*> pluginmenus;

    QString logfilename;

    Workspace *workspace;
};
