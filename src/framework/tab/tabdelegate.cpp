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

#include <QApplication>
#include <QMetaType>

#include "tabdelegate.h"

template TabDelegate* Singleton<TabDelegate>::getInstance();

void TabDelegate::addTab(const QString& callernamespace, QWidget* widget, const QString& name) {
  QMetaObject::invokeMethod(qApp, "addTab", Qt::QueuedConnection, Q_ARG(QString, callernamespace), Q_ARG(QWidget*, widget), Q_ARG(QString, name));
}

void TabDelegate::addToolBar(const QString& callernamespace, QToolBar* toolbar) {
  qRegisterMetaType<QToolBar*>("QToolBar*");
  QMetaObject::invokeMethod(qApp, "addToolBar", Qt::QueuedConnection, Q_ARG(QString, callernamespace), Q_ARG(QToolBar*, toolbar));
}

void TabDelegate::setTabName(const QString& callernamespace, const QString& text) {
  QMetaObject::invokeMethod(qApp, "setTabName", Qt::QueuedConnection, Q_ARG(QString, callernamespace), Q_ARG(QString, text));
}

void TabDelegate::setActiveTab(const QString& callernamespace) {
  QMetaObject::invokeMethod(qApp, "setActiveTab", Qt::QueuedConnection, Q_ARG(QString, callernamespace));
}

bool TabDelegate::isTabActive(const QString &callernamespace) {
  bool rc;
  QMetaObject::invokeMethod(qApp, "isTabActive", Qt::AutoConnection, Q_RETURN_ARG(bool, rc), Q_ARG(QString, callernamespace));
  return rc;
}
