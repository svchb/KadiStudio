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

#include "dockwindow.h"

DockWindow::DockWindow(QDockWidget *parent)
    : QDockWidget(parent) {
  this->dockarea = Qt::RightDockWidgetArea;
}

DockWindow::~DockWindow() {
}

QDockWidget* DockWindow::widget() {
  return this;
}

Qt::DockWidgetArea DockWindow::area() {
  return dockarea;
}

void DockWindow::addWidget(QWidget* toolChild, Qt::DockWidgetArea area, Qt::WindowFlags flags) {
  // set the right title
  if (toolChild->windowTitle().isEmpty()) {
    QDockWidget::setWindowTitle(toolChild->objectName());
  } else {
    QDockWidget::setWindowTitle(toolChild->windowTitle());
  }

  this->dockarea = area;
  QDockWidget::setWidget(toolChild);
  QDockWidget::setWindowFlags(flags);
}
