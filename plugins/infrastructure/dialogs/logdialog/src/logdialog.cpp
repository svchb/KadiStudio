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

#include <cmath>

#include <QDebug>
#include <QFile>
#include <QApplication>
#include <QScreen>
#include <QVBoxLayout>
#include <QDialogButtonBox>
#include <QLabel>
#include <QMessageBox>
#include <QSplitter>
#include <QJsonDocument>
#include <QJsonObject>
#include <QTimer>

#include <framework/pluginframework/pluginmanagerinterface.h>
#include <plugins/infrastructure/workflows/processmanager/processmanagerinterface.h>

#include "logtextwidget.h"
#include "logtreeitem.h"
#include "logline.h"
#include "logdialog.h"


LogDialog::LogDialog(LibFramework::PluginManagerInterface* pluginmanager)
    : QDialog(nullptr, Qt::WindowTitleHint | Qt::WindowSystemMenuHint), error_label(new QLabel()),
      current_workflow_id(-1), log_seek(0), incomplete_last_line(nullptr), root(nullptr), tree_view_enabled(false) {

  processmanager_interface = pluginmanager->getInterface<ProcessManagerInterface*>("/plugins/infrastructure/workflows/processmanager");

  QScreen *screen = qApp->primaryScreen();
  QRect primaryScreenSize = screen->geometry();
  int width = (int) std::round(primaryScreenSize.width() * 0.7);
  int height = (int) std::round(primaryScreenSize.height() * 0.7);
  resize(width, height);
  setModal(false);
  setWindowFlags(Qt::Window);
  auto *layout = new QVBoxLayout(this);
  log_text_widget = new LogTextWidget();
  log_text_widget->setReadOnly(true);
  logtree = new QTreeWidget();
  logtree->setHeaderLabels({"Log context", "Order"});
  logtree->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
  logtree->sortByColumn(1, Qt::AscendingOrder);
  logtree->hideColumn(1);
  logtree->setSortingEnabled(true);
  logtree->setVisible(false); // will be enabled once context tree information is provided via `setContextTree()`
  connect(logtree, &QTreeWidget::itemSelectionChanged, this, &LogDialog::selectedContextChanged);
  auto *splitter = new QSplitter();
  splitter->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
  splitter->addWidget(logtree);
  splitter->addWidget(log_text_widget);
  splitter->setStretchFactor(1, 2);
  layout->addWidget(splitter);

  auto *buttonContainer = new QWidget();
  auto *buttonContainerLayout = new QHBoxLayout();
  auto *buttonBox = new QDialogButtonBox(QDialogButtonBox::Close);
  buttonContainer->setLayout(buttonContainerLayout);
  error_label->setStyleSheet("QLabel { color: red };");
  error_label->setMaximumHeight(30);
  buttonContainerLayout->addWidget(error_label);
  buttonContainerLayout->addWidget(buttonBox);
  connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::close);
  layout->addWidget(buttonContainer);
  setLayout(layout);

  log_watcher = new QFileSystemWatcher(this);
  tree_watcher = new QFileSystemWatcher(this);
  connect(log_watcher, &QFileSystemWatcher::fileChanged, this, &LogDialog::updateLog);
  connect(tree_watcher, &QFileSystemWatcher::fileChanged, this, &LogDialog::updateTree);
}

QString LogDialog::getText() {
  return log_text_widget->toPlainText();
}

void LogDialog::open() {
  QDialog::open();

  // bring window to front (see https://stackoverflow.com/a/10808934/3997725)
  setWindowState( (windowState() & ~Qt::WindowMinimized) | Qt::WindowActive);
  raise();  // for MacOS
  activateWindow(); // for Windows

  log_text_widget->ensureCursorVisible(); // to enable autoscroll initially
}


void LogDialog::showLogDialog(int workflow_id) {
  if (log_path.isEmpty() || tree_path.isEmpty() || workflow_id != current_workflow_id) {
    if (!log_path.isEmpty()) {
      log_watcher->removePath(log_path);
    }
    if (!tree_path.isEmpty()) {
      tree_watcher->removePath(tree_path);
    }
    log_path = processmanager_interface->retrieveWorkflowLogPath(workflow_id);
    if (!log_watcher->addPath(log_path)) {
      fileNotFound(log_path);
      return;
    }
    try {
      tree_path = processmanager_interface->retrieveWorkflowTreePath(workflow_id);
      if (!tree_watcher->addPath(tree_path)) {
        setError(tr("Unable to set up updater for status tree information at \"%1\"").arg(tree_path));
      }
    } catch (const std::exception& e) {
      QString error = tr("Unable to receive path to status tree information: %1").arg(e.what());
      qDebug() << "LogDialog: " << error;
      // not showing an error here for backwards compatibility for now
      // setError(error);
    }
    log_seek = 0;
    setWindowTitle(tr("Workflow %1 Execution Log").arg(workflow_id));
    current_workflow_id = workflow_id;
    logtree->clear();
    log_content.clear();
    log_text_widget->clear();
    initContextTree(tree_path);
  }

  if (updateLog(log_path)) {
    open();
  }
  if (tree_view_enabled) {
    updateTree(tree_path);
    QTimer::singleShot(1000, [this] () {
      updateTree(tree_path);
    });
  }
}

bool LogDialog::isOpen() {
  return isVisible();
}

LogLine* LogDialog::addLogLine(const QString& line) {
  LogLine *result = nullptr;
  if (line.isEmpty()) return result;
  QString log_context, log_message;
  if (line.count(";") < 1) {
    log_context = QString::fromStdString(DEFAULT_CONTEXT);
    log_message = line;
  } else {
    const QChar separator = ';';
    log_context = line.section(separator, 0, 0).trimmed();
    log_message = line.section(separator, 1);
  }

  if (!log_message.isEmpty()) {
    result = getOrInsertLogContent(log_context.toStdString())->add(log_message.toStdString());
    if (tree_view_enabled) {
      auto item_iter = tree_items.find(log_context.toStdString());
      if (item_iter == tree_items.end()) {
        QJsonObject updated_tree = loadStatusTree(tree_path);
        bool item_was_added = false;
        if (!updated_tree.isEmpty()) {
          addTreeItems(updated_tree, {log_context});
          item_was_added = (tree_items.find(log_context.toStdString()) != tree_items.end());
        }
        if (!item_was_added) {
          // fallback, it does not seem to be part of the workflow hierarchy
          auto item = new LogTreeItem(log_context, log_context);
          logtree->addTopLevelItem(item);
          tree_items[log_context.toStdString()] = item;
        }
      }
    }

  }
  return result;
}

void LogDialog::initContextTree(const QString& path) {
  logtree->clear();
  tree_items.clear();

  bool enable_tree = !path.isEmpty();
  QJsonObject tree;
  if (enable_tree) {
    tree = loadStatusTree(path);
    enable_tree = !tree.isEmpty();
  }
  setTreeViewEnabled(enable_tree);
  if (enable_tree) {
    logtree->setVisible(true);
    root = new LogTreeItem("Workflow", QString::fromStdString(WORKFLOW_ROOT_CONTEXT));
    logtree->addTopLevelItem(root);

    addTreeItems(tree, tree.keys()); // add all items from the json tree info
    logtree->expandAll();
    root->setSelected(true);
  }
}

void LogDialog::addTreeItems(const QJsonObject& tree, const QStringList& keys) {
  std::vector<QString> orphans;
  for (const QString& key : keys) {
    QJsonObject node = tree[key].toObject();
    if (node.isEmpty()) continue; // not in the tree -> ignore key
    else if (node.contains("parent")) {
      orphans.push_back(key);
    } else {
      auto item = new LogTreeItem(node["name"].toString(), key);
      root->addChild(item);
      if (node.contains("state")) {
        item->setState(node["state"].toString());
      }
      item->setOrder(node["order"].toInt(0));
      tree_items[key.toStdString()] = item;

      // add parent relation to LogContent as well (needed to add refs to the parents when adding the line)
      getOrInsertLogContent(key.toStdString())->setParent(getOrInsertLogContent(WORKFLOW_ROOT_CONTEXT));
    }
  }

  for (const QString &orphan_key : orphans) {
    const QJsonObject &orphan = tree[orphan_key].toObject();
    auto parent_iter = tree_items.find(orphan["parent"].toString().toStdString());
    if (parent_iter != tree_items.end()) {
      auto item = new LogTreeItem(orphan["name"].toString(), orphan_key);
      item->setOrder(orphan["order"].toInt(0));
      parent_iter->second->addChild(item);
      tree_items[orphan_key.toStdString()] = item;

      // add parent relation to LogContent as well (needed to add refs to the parents when adding the line)
      getOrInsertLogContent(orphan_key.toStdString())->setParent(getOrInsertLogContent(parent_iter->first));
    }
  }
}

bool LogDialog::updateLog(const QString& file_path) {
  QFile file(file_path);
  if (!file.open(QIODevice::ReadOnly)) {
    fileNotFound(file_path);
    log_watcher->removePath(file_path);
    close();
    return false;
  }
  if (log_seek > 0) {
    file.seek(log_seek);
    if (incomplete_last_line) {
      QByteArray raw = file.readLine();
      std::string line = raw.toStdString();
      incomplete_last_line->append(line); // make the last line complete
      incomplete_last_line = nullptr;
    }
  }
  qint64 chars_read = 0;
  LogLine *last_line = nullptr;
  while (!file.atEnd()) {
    const QString& line = file.readLine();
    last_line = addLogLine(line);
    chars_read += line.size();
  }
  if (last_line && !last_line->isEmpty() && last_line->getLastChar() != '\n') {
    incomplete_last_line = last_line;
  }
  log_seek += chars_read;
  file.seek(log_seek);
  return true;
}

void LogDialog::updateTree(const QString& file_path) {
  QJsonObject updated_tree = loadStatusTree(file_path);
  QStringList new_keys;
  for (const QString& key : updated_tree.keys()) {
    auto existing_tree_item = tree_items.find(key.toStdString());
    if (existing_tree_item == tree_items.end()) {
      new_keys.push_back(key);
    } else {
      LogTreeItem *item = existing_tree_item->second;
      const QJsonObject &updated_node = updated_tree[key].toObject();
      item->setName(updated_node["name"].toString());
      item->setOrder(updated_node["order"].toInt(0));
      if (updated_node.contains("state")) {
        item->setState(updated_node["state"].toString());
      }
    }
  }
  addTreeItems(updated_tree, new_keys); // add only new items
}

void LogDialog::selectedContextChanged() {
  auto selectedItems = logtree->selectedItems();
  log_text_widget->clear();
  if (!selectedItems.isEmpty()) {
    QString selected = dynamic_cast<LogTreeItem*>(selectedItems[0])->getId();
    LogContent *content = getOrInsertLogContent(selected.toStdString());
    QString log_line = removeTrailingNewline(content->toString()); // QTextEdit::append() adds a newline
    log_text_widget->setCurrentContent(content);
    log_text_widget->setTextTermFormatting(log_line);
    log_text_widget->ensureCursorVisible();
  }
}

void LogDialog::logContentUpdated(const std::string& context, const LogLine& line) {
  QString log_line = removeTrailingNewline(line.toString()); // QTextEdit::append() adds a newline
  if (tree_view_enabled) {
    auto selectedItems = logtree->selectedItems();
    if (!selectedItems.isEmpty()) {
      std::string selected = dynamic_cast<LogTreeItem*>(selectedItems[0])->getId().toStdString();
      if (selected == context) {
        log_text_widget->setTextTermFormatting(log_line);
      }
    }
  } else {
    log_text_widget->setTextTermFormatting(log_line);
  }
}

LogContent* LogDialog::getOrInsertLogContent(const std::string& context) {
  auto result = log_content.find(context);
  if (result == log_content.end()) {
    result = log_content.emplace(context, LogContent {context, this}).first;
  }
  return &result->second;
}

std::string LogDialog::getLogForContext(const std::string& context) const {
  auto content = log_content.find(context);
  if (content != log_content.end()) {
    return content->second.toString();
  }
  return {};
}

QJsonObject LogDialog::loadStatusTree(const QString &path) {
  QJsonObject result;
  QFile file(path);
  if (!file.open(QIODevice::ReadOnly)) {
    setError(tr("Error opening file with tree information at %1").arg(path));
    log_watcher->removePath(path);
    return result;
  }
  QJsonDocument doc;
  doc = QJsonDocument::fromJson(file.readAll());
  if (!doc.isNull() && doc.isObject()) {
    result = doc.object();
  }
  return result;
}

void LogDialog::setTreeViewEnabled(bool enabled) {
  tree_view_enabled = enabled;
  logtree->setEnabled(enabled);
}

void LogDialog::fileNotFound(const QString& path) {
  QMessageBox::critical(this, tr("Unable to read log file"),
                              tr("Can not read the log file from %1. Please check your installation!").arg(path));
}

QString LogDialog::removeTrailingNewline(const std::string& str) {
  // NOT NEEDED FOR colorterminalwidget
  // std::string result = str;
  // if (!result.empty() && result.back() == '\n') {
  //   result.pop_back();
  // }
  // return QString::fromStdString(result);
  return QString::fromStdString(str);
}

void LogDialog::setError(const QString& error_message) {
  error_label->setText(error_message);
}
