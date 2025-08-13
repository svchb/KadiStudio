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

#include <QMultiMap>
#include <QTabWidget>
#include <QToolBar>

#include <framework/pluginframework/pluginmanagerinterface.h>
#include <cpputils/dllapi.hpp>


/**
 * @brief      The workspace in which the plugins are loaded inside the mainwindow.
 * @ingroup    framework
 */
class DLLAPI Workspace final : public QTabWidget {
  Q_OBJECT

  public:
    Workspace();
    ~Workspace() override;

    void addTab(const QString& callernamespace, QWidget* widget, const QString& name);
    void addToolBar(const QString& callernamespace, QToolBar* toolbar);
    void setTabName(const QString& callernamespace, const QString& text);
    void setActiveTab(const QString& callernamespace);
    bool isTabActive(const QString &callernamespace);
    void removeTab(const QString& callernamespace);
    QString getActiveNamespace();

    QWidget* getWidget();

  private Q_SLOTS:
    void tabClosed(int index);
    void tabChanged(int index);

  private:

    QMultiMap<QString, QWidget*> plugintabs;
    LibFramework::PluginManagerInterface *pluginmanager;

};
