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
  for (int i = 0; i < plugintabs.count(); i++) {
    tabClosed(i);
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

void Workspace::addTab(const QString& callernamespace, QWidget* widget, const QString& name) {
  QWidget *tabwidget = new QWidget;
  QVBoxLayout *layout = new QVBoxLayout;
  tabwidget->setLayout(layout);
  layout->insertWidget(1, widget);
  plugintabs.insert(callernamespace, tabwidget);
  QTabWidget::addTab(tabwidget, name);
  QTabWidget::setCurrentWidget(tabwidget);
}

void Workspace::addToolBar(const QString& callernamespace, QToolBar* toolbar) {
  QWidget *tab = plugintabs.value(callernamespace);
  if (!tab) {
    qDebug("Workspace: Can't add toolbar to the tab in namespace %s. Please add a tab first.", qPrintable(callernamespace));
    return;
  }
  QVBoxLayout *layout = qobject_cast<QVBoxLayout*>(tab->layout());
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

void Workspace::tabClosed(int index) {
  MainWindow *mainwindow = MainWindow::getInstance();
  QWidget *widget = QTabWidget::widget(index);
  const QString &namespacepath = plugintabs.key(widget);
  pluginmanager->unload(namespacepath.toStdString());
  if (pluginmanager->isUnloaded(namespacepath.toStdString())) {
    mainwindow->removePluginItems(namespacepath);
  }

  const LibFramework::PluginInfo *plugininfo = pluginmanager->getPluginInfos(namespacepath.toStdString()).at(0);
  std::set<std::string> requiredpluginnamespaces = plugininfo->getRequiredNamespaces();
  for (const std::string& requiredpluginnamespace : requiredpluginnamespaces) {
    if (pluginmanager->isUnloaded(requiredpluginnamespace)) {
      mainwindow->removePluginItems(QString::fromStdString(requiredpluginnamespace));
    }
  }
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
