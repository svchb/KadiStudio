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

#include "qvtiwidget_list.h"


QVTIListWidget::QVTIListWidget(AbstractValueTypeInterface* avti, QWidget* parent)
    : QVTIWidget(avti, new QListWidget(parent)) {
}

void QVTIListWidget::synchronizeVTI() {
  getQListWidget()->clear();
  const std::vector<std::string> &values = getValue<std::vector<std::string>>();
  for (const std::string& text : values) {
    getQListWidget()->addItem(QString::fromStdString(text));
  }
}
