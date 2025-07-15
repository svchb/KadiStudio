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

#include <QObject>
#include <QList>

#include <framework/pluginframework/pluginmanagerinterface.h>

#include <framework/dock/dockdelegate.h>
#include "nodes/models/workflownode.h"
#include "workflowview.h"
#include "workfloweditorinterface.h"

class QToolBar;


/**
 * @class      Creates the workfloweditor.
 * @ingroup    workfloweditor
 */
class Workfloweditor : public QObject, public WorkflowEditorInterface {
    Q_OBJECT

  public:
    explicit Workfloweditor(LibFramework::PluginManagerInterface* pluginmanager, QObject* parent = NULL);

    void addTab();
    void addMenus();

  Q_SIGNALS:
    void nodesSelected(bool value);

  private Q_SLOTS:
    void openWorkflow(const QString& path) override;
    void setTabName(const QString& name);

  private:
    QMenu* createWorkflowMenu();
    QMenu* createViewMenu();
    QMenu* createToolsMenu() const;
    QToolBar* createToolBar(QWidget* parent);

    void showKadiConfig();
    void registerTool();

    LibFramework::PluginManagerInterface *pluginmanager;
    WorkflowView *view;
    WorkflowScene *scene;
    QMenu *workflowMenu;
    QMenu *viewMenu;
    QMenu *toolsMenu;
    QToolBar *toolbar;

};
