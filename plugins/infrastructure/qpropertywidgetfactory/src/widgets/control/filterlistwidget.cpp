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

#include <framework/enhanced/qlineeditclearable.h>
#include <QVBoxLayout>

#include "filterlistwidget.h"

FilterListWidget::FilterListWidget(const QString& placeholdertext, QWidget* parent)
    : QWidget(parent) {

  QVBoxLayout *widgetlayout = new QVBoxLayout(this);

  QLineEditClearable *filterwidget = new QLineEditClearable();
  filterwidget->setPlaceholderText(placeholdertext);
  widgetlayout->addWidget(filterwidget);

  listwidget = new QListWidget(this);
  widgetlayout->addWidget(listwidget);

  connect(filterwidget, SIGNAL(textChanged(const QString&)), this, SLOT(filterList(const QString&)));
  connect(listwidget, SIGNAL(itemClicked(QListWidgetItem*)), this, SIGNAL(itemClicked(QListWidgetItem*)));
  connect(listwidget, SIGNAL(itemSelectionChanged()), this, SIGNAL(itemSelectionChanged()));
}

void FilterListWidget::filterList(const QString& text) {
  QListWidgetItem *item;
  for (int i = 0; i < listwidget->count(); i++) {
    item = listwidget->item(i);
    QString extension = item->text();
    if (extension.contains(text)) {
      item->setHidden(false);
    } else {
      item->setHidden(true);
    }
  }
}
