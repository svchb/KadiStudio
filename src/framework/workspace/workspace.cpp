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

#include <QVBoxLayout>

#include <pluginframework/pluginmanager.h>
#include <mainwindow/mainwindow.h>

#include "workspace.h"

Workspace::Workspace() : QTabWidget(nullptr) {
  this->pluginmanager = LibFramework::PluginManager::getInstance();
  setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
  setTabsClosable(true);
  connect(this, SIGNAL(tabCloseRequested(int)), this, SLOT(tabClosed(int)));
  connect(this, SIGNAL(currentChanged(int)), this, SLOT(tabChanged(int)));
}

Workspace::~Workspace() {
  while (count() > 0) {
    tabClosed(count() - 1);
  }
}

QWidget* Workspace::getWidget() {
  return this;
}

void Workspace::tabChanged(int index) {
  QWidget *widget = QTabWidget::widget(index);
  QString namespacepath = plugintabs.key(widget);
  MainWindow *mainwindow = MainWindow::getInstance();
  mainwindow->showPluginItems(namespacepath);
}

void Workspace::addTab(const QString& callernamespace, QWidget* widget, const QString& name)
{
  if (!widget) {
    qWarning() << "Workspace::addTab: nullptr widget for" << callernamespace;
    return;
  }

  // Outer container for the tab
  auto *tabwidget = new QWidget(this);
  auto *layout    = new QVBoxLayout(tabwidget);
  layout->setContentsMargins(0, 0, 0, 0);
  layout->setSpacing(0);

  widget->setParent(tabwidget);
  layout->addWidget(widget);

  plugintabs.insert(callernamespace, tabwidget);

  // Add & activate
  const int idx = QTabWidget::addTab(tabwidget, name);
  QTabWidget::setCurrentIndex(idx);
}

void Workspace::addToolBar(const QString& callernamespace, QToolBar* toolbar) {
  QWidget *tab = plugintabs.value(callernamespace);
  if (!tab) {
    qDebug("Workspace: Can't add toolbar to the tab in namespace %s. Please add a tab first.", qPrintable(callernamespace));
    return;
  }
  auto* layout = qobject_cast<QVBoxLayout*>(tab->layout());
  layout->insertWidget(0, toolbar);
}

void Workspace::setTabName(const QString& callernamespace, const QString& text) {
  QWidget *tab = plugintabs.value(callernamespace);
  if (!tab) {
    qDebug("Workspace: Can't change the name of the tab in namespace %s. Please add a tab first.", qPrintable(callernamespace));
    return;
  }
  int index = QTabWidget::indexOf(tab);
  QTabWidget::setTabText(index, text);
}

void Workspace::setActiveTab(const QString &callernamespace) {
  QWidget *tab = plugintabs.value(callernamespace);
  if (!tab) {
    qDebug("Workspace: Can't set the active tab to tab in namespace %s. Please add a tab first.", qPrintable(callernamespace));
    return;
  }
  int index = QTabWidget::indexOf(tab);
  QTabWidget::setCurrentIndex(index);
}

bool Workspace::isTabActive(const QString &callernamespace) {
  QWidget *tab = plugintabs.value(callernamespace);
  if (!tab) {
    return false;
  }
  int index = QTabWidget::indexOf(tab);
  return QTabWidget::currentIndex() == index;
}

void Workspace::tabClosed(int index)
{
  if (index < 0 || index >= count())
    return;

  MainWindow *mainwindow = MainWindow::getInstance();

  QWidget *page = QTabWidget::widget(index);
  if (!page) {
    // Just remove the tab frame if something went wrong
    QTabWidget::removeTab(index);
    return;
  }

  // Find the namespace key for this page
  const QString ns = plugintabs.key(page);
  if (ns.isEmpty()) {
    // Mapping not found; still remove the tab safely
    QTabWidget::removeTab(index);
    page->deleteLater();
    return;
  }

  // Unload plugin and update UI safely
  pluginmanager->unload(ns.toStdString());

  if (pluginmanager->isUnloaded(ns.toStdString()) && mainwindow) {
    mainwindow->removePluginItems(ns);
  }

  // Guard against empty info list!
  const auto &infos = pluginmanager->getPluginInfos(ns.toStdString());
  if (!infos.empty() && infos[0] != nullptr) {
    const std::set<std::string> required = infos[0]->getRequiredNamespaces();
    if (mainwindow) {
      for (const std::string &rns : required) {
        if (pluginmanager->isUnloaded(rns)) {
          mainwindow->removePluginItems(QString::fromStdString(rns));
        }
      }
    }
  }

  // Keep bookkeeping in sync and destroy the page
  plugintabs.remove(ns);
  QTabWidget::removeTab(index);
  page->deleteLater();
}

void Workspace::removeTab(const QString& callernamespace) {
  if (!plugintabs.contains(callernamespace)) {
    return;
  }
  QWidget *widget = plugintabs.take(callernamespace);
  int index = QTabWidget::indexOf(widget);
  QTabWidget::removeTab(index);
  delete widget;
}

QString Workspace::getActiveNamespace() {
  QWidget *widget = QTabWidget::currentWidget();
  if (widget == nullptr) return "";
  const QString &namespacepath = plugintabs.key(widget);
  return namespacepath;
}
