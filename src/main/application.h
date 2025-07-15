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

#pragma once

#include <QApplication>
#include <QToolBar>
#include <QMenu>
#include <QDir>

#include <framework/pluginframework/pluginmanagerinterface.h>

class DockWindow;

/**
 * @brief      Creates an instance of the kadistudio application.
 * @ingroup    framework
 */
class Application : public QApplication {
    Q_OBJECT

  public:
    /**
      * @param argc must be a reference
      */
    Application(const QString& appname, int& argc, char** argv);
    virtual ~Application();

    int evaluateHelp(int argc, char** argv);

  public Q_SLOTS:
    void showMainWindow();

    void addTab(const QString& callernamespace, QWidget* widget, const QString& name);
    void addToolBar(const QString& callernamespace, QToolBar* toolbar);
    void setTabName(const QString& callernamespace, const QString& text);
    void setActiveTab(const QString& callernamespace);
    bool isTabActive(const QString &callernamespace);

    void addMenu(const QString& callernamespace, QMenu* menu);
    void showMessage(const QString& orgin, const QString& msg, int level);
    void clearMessage();

  private:
    void loadLocalPlugins();

    LibFramework::PluginManagerInterface *pluginmanager;
    bool autostart;

};
