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

#include <unordered_map>
#include <QFileSystemWatcher>
#include <QDialog>
#include <QPlainTextEdit>

#include "../logdialoginterface.h"
#include "logcontent.h"

class LogTextWidget;
class QTreeWidget;
class QTreeWidgetItem;
class ProcessManagerInterface;
namespace LibFramework {
  class PluginManagerInterface;
}
class LogTreeItem;
class QLabel;

/**
 * @brief      Implementation of the LogDialog
 * @ingroup    dialog
 */
class LogDialog : public QDialog, public LogDialogInterface {
  Q_OBJECT

  public:
    explicit LogDialog(LibFramework::PluginManagerInterface* pluginmanager);
    QString getText();

    void open() override;

    void showLogDialog(int workflow_id) override;
    bool isOpen() override;

  Q_SIGNALS:
    void updatedLog(const QString& context, const LogLine& line);

  private Q_SLOTS:
    bool updateLog(const QString& file_path);
    void updateTree(const QString& file_path);
    void setTreeViewEnabled(bool enabled);
    void selectedContextChanged();

  private:
    void initContextTree(const QString& path);
    LogLine* addLogLine(const QString& line);
    void addTreeItems(const QJsonObject& tree, const QStringList& keys);
    LogContent* getOrInsertLogContent(const std::string& context);
    std::string getLogForContext(const std::string& context) const;

    void logContentUpdated(const std::string& context, const LogLine& line);
    QJsonObject loadStatusTree(const QString& path);
    void fileNotFound(const QString& path);
    static QString removeTrailingNewline(const std::string& str);
    void setError(const QString& error_message);

    LogTextWidget *log_text_widget;
    QTreeWidget *logtree;
    QLabel *error_label;

    std::unordered_map<std::string, LogContent> log_content;
    std::unordered_map<std::string, LogTreeItem*> tree_items;

    ProcessManagerInterface *processmanager_interface;
    int current_workflow_id;
    QString log_path;
    QString tree_path;
    QFileSystemWatcher *log_watcher;
    QFileSystemWatcher *tree_watcher;
    qint64 log_seek;
    LogLine *incomplete_last_line;
    LogTreeItem *root;
    bool tree_view_enabled;

    const std::string DEFAULT_CONTEXT = "Process Engine";
    const std::string WORKFLOW_ROOT_CONTEXT = "Workflow";

    friend class LogContent;
};
