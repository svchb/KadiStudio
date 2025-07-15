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
#include <QPushButton>

#include <properties/data/properties/controls/activesetcontrolproperty.h>

#include "listswidget.h"
#include "filterlistwidget.h"

ListsWidget::ListsWidget(Property* property, QWidget* parent)
    : QPropertyWidget(property, new QGroupBox(property->getName().c_str(), parent)) {

  QHBoxLayout *widgetlayout = new QHBoxLayout(getWidget());

  activelistwidget = new FilterListWidget("Type to filter active entries", getWidget());
  activelistwidget->getQListWidget()->setSelectionMode(QAbstractItemView::SelectionMode::ExtendedSelection);
  widgetlayout->addWidget(activelistwidget);

  QVBoxLayout *buttonslayout = new QVBoxLayout();
  activate_all_button = new QPushButton("<<");
  activate_selected_button = new QPushButton("<");
  kick_selected_button = new QPushButton(">");
  kick_all_button = new QPushButton(">>");
  buttonslayout->addStretch(0);
  buttonslayout->addWidget(activate_all_button);
  buttonslayout->addWidget(activate_selected_button);
  buttonslayout->addWidget(kick_selected_button);
  buttonslayout->addWidget(kick_all_button);
  buttonslayout->addStretch(0);
  widgetlayout->addLayout(buttonslayout);

  kickedlistwidget = new FilterListWidget(tr("Type to filter kicked entries"), getWidget());
  kickedlistwidget->getQListWidget()->setSelectionMode(QAbstractItemView::SelectionMode::ExtendedSelection);

  widgetlayout->addWidget(kickedlistwidget);

  connect(activelistwidget, &FilterListWidget::itemSelectionChanged, [&]() {
    kick_selected_button->setDisabled(activelistwidget->getQListWidget()->selectedItems().isEmpty());
  });
  connect(kickedlistwidget, &FilterListWidget::itemSelectionChanged, [&]() {
    activate_selected_button->setDisabled(kickedlistwidget->getQListWidget()->selectedItems().isEmpty());
  });
  connect(activate_all_button, &QPushButton::pressed, this, &ListsWidget::activeAll);
  connect(activate_selected_button, &QPushButton::pressed, this, &ListsWidget::activateSelected);
  connect(kick_selected_button, &QPushButton::pressed, this, &ListsWidget::kickSelected);
  connect(kick_all_button, &QPushButton::pressed, this, &ListsWidget::kickAll);
}

ListsWidget::~ListsWidget() {
}

void ListsWidget::synchronizeVTI() {
  ActiveSetControlProperty *activesetcontrolproperty = getProperty<ActiveSetControlProperty>();
  activelistwidget->getQListWidget()->clear();
  for (auto element : activesetcontrolproperty->getActiveElements()) {
    activelistwidget->getQListWidget()->addItem(QString::fromStdString(element));
  }
  kickedlistwidget->getQListWidget()->clear();
  for (auto element : activesetcontrolproperty->getDeactiveElements()) {
    kickedlistwidget->getQListWidget()->addItem(QString::fromStdString(element));
  }

  activate_all_button->setDisabled(kickedlistwidget->getQListWidget()->count() == 0);
  kick_all_button->setDisabled(activelistwidget->getQListWidget()->count() == 0);
  activate_selected_button->setDisabled(true);
  kick_selected_button->setDisabled(true);
}

void ListsWidget::activateSelected() {
  ActiveSetControlProperty *activeset = getProperty<ActiveSetControlProperty>();
  std::vector<std::string> itemstoactivate;
  itemstoactivate.reserve(kickedlistwidget->getQListWidget()->selectedItems().size());
  for (auto item : kickedlistwidget->getQListWidget()->selectedItems()) {
    itemstoactivate.push_back(item->text().toStdString());
  }
  activeset->activateElementsByCondition(
  [itemstoactivate](const std::string& elementname) -> bool {
    return std::binary_search(itemstoactivate.begin(), itemstoactivate.end(), elementname);
  });
}

void ListsWidget::kickSelected() {
  ActiveSetControlProperty *activeset = getProperty<ActiveSetControlProperty>();
  std::vector<std::string> itemstokick;
  itemstokick.reserve(kickedlistwidget->getQListWidget()->selectedItems().size());
  for (auto item : activelistwidget->getQListWidget()->selectedItems()) {
    itemstokick.push_back(item->text().toStdString());
  }
  activeset->deactivateElementsByCondition(
  [itemstokick](const std::string& elementname) -> bool {
    return std::binary_search(itemstokick.begin(), itemstokick.end(), elementname);
  });
}

void ListsWidget::activeAll() {
  ActiveSetControlProperty *activeset = getProperty<ActiveSetControlProperty>();
  activeset->activateAllElements();
}

void ListsWidget::kickAll() {
  ActiveSetControlProperty *activeset = getProperty<ActiveSetControlProperty>();
  activeset->deactivateAllElements();
}
