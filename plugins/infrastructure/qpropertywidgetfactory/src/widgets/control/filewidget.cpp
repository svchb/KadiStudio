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

#include <QGroupBox>
#include <QVBoxLayout>
#include <QListWidgetItem>

#include <properties/data/ambassador.h>

#include "filewidget.h"
#include "filterlistwidget.h"

FileWidget::FileWidget(Property* property, QWidget* parent)
    : QPropertyWidget(property, new QGroupBox(QString::fromStdString(property->getName()), parent)) {

  QVBoxLayout *widgetlayout = new QVBoxLayout(getWidget());

  filterlistwidget = new FilterListWidget(tr("Type to filter entries"), getWidget());
  filterlistwidget->setFixedWidth(250);
  widgetlayout->addWidget(filterlistwidget);

  widgetlayout->addStretch(0);

  connect(filterlistwidget, &FilterListWidget::itemClicked, this, [&](QListWidgetItem* item) {
    std::string selected = item->text().toStdString();
    getInnerAmbassador()->setValue<std::string>("selected", selected);
  });
}

void FileWidget::synchronizeVTI() {
  QListWidget *listwidget = filterlistwidget->getQListWidget();
  listwidget->clear();
  const std::vector<std::string> &elements = getInnerAmbassador()->getValue<std::vector<std::string>>("elements");
  for (auto element : elements) {
    listwidget->addItem(QString::fromStdString(element));
  }
  const std::string &selected = getInnerAmbassador()->getValue<std::string>("selected");
  if (!selected.empty()) {
    for (int i = 0; i < listwidget->count(); i++) {
      QListWidgetItem *item = listwidget->item(i);
      if (item->text() == QString::fromStdString(selected)) {
        listwidget->setCurrentItem(item);
        break;
      }
    }
  }
}
