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

#include <QWidget>
#include <QHBoxLayout>
#include <QSpacerItem>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QAction>
#include <QListWidget>
#include <qlayoutitem.h>

#include "toolbookmarksdialog.h"

ToolBookmarksDialog::ToolBookmarksDialog(QList<QString>& toolbookmarks, QWidget* parent) : QDialog(parent) {

  setWindowTitle(tr("Manage Bookmarks"));

  setMinimumWidth(1000);
  setMinimumHeight(400);

  QVBoxLayout *layout = new QVBoxLayout(this);

  QWidget *widget = new QWidget();
  QHBoxLayout *listlayout = new QHBoxLayout();

  listwidget = new QListWidget();
  widget->setLayout(listlayout);
  for (auto toolbookmark : toolbookmarks) {
    listwidget->addItem(toolbookmark);
  }
  listlayout->addWidget(listwidget);

  QWidget *buttonwidget = new QWidget();
  QVBoxLayout *buttonlayout = new QVBoxLayout();
  QPushButton *upbutton = new QPushButton();
  upbutton->setIcon(style()->standardIcon(QStyle::SP_ArrowUp));
  connect(upbutton, &QPushButton::clicked, [this]() {
    int currentrow = listwidget->currentRow();
    if (currentrow == 0) {
      return;
    }
    QListWidgetItem *entry = listwidget->takeItem(currentrow);
    listwidget->insertItem(currentrow-1, entry);
    listwidget->setCurrentItem(entry);
  });
  QPushButton *downbutton = new QPushButton();
  downbutton->setIcon(style()->standardIcon(QStyle::SP_ArrowDown));
  connect(downbutton, &QPushButton::clicked, [this]() {
    int currentrow = listwidget->currentRow();
    if (currentrow >= listwidget->count()-1) {
      return;
    }
    QListWidgetItem *entry = listwidget->takeItem(currentrow);
    listwidget->insertItem(currentrow+1, entry);
    listwidget->setCurrentItem(entry);
  });
  QPushButton *deletebutton = new QPushButton();
  deletebutton->setIcon(style()->standardIcon(QStyle::SP_DialogDiscardButton));
  connect(deletebutton, &QPushButton::clicked, [this]() {
    QListWidgetItem *entry = listwidget->currentItem();
    if (entry) {
      listwidget->removeItemWidget(entry);
      delete entry;
    }
  });
  buttonlayout->addWidget(upbutton);
  buttonlayout->addWidget(downbutton);
  buttonlayout->addItem(new QSpacerItem(0, 0, QSizePolicy::Fixed, QSizePolicy::Expanding));
  buttonlayout->addWidget(deletebutton);
  buttonwidget->setLayout(buttonlayout);
  widget->layout()->addWidget(buttonwidget);
  layout->addWidget(widget);
  QDialogButtonBox *dialogbuttonbox = new QDialogButtonBox();
  QPushButton *selectbutton = dialogbuttonbox->addButton(tr("Select"), QDialogButtonBox::ActionRole);
  dialogbuttonbox->addButton(QDialogButtonBox::Ok);
  dialogbuttonbox->addButton(QDialogButtonBox::Reset);
  dialogbuttonbox->addButton(QDialogButtonBox::Cancel);
  layout->addWidget(dialogbuttonbox);

  setLayout(layout);

  connect(selectbutton, &QPushButton::clicked, [this]() {
    if (listwidget->currentItem() != nullptr) {
      QString toolidentificationstring = listwidget->currentItem()->text();
      Q_EMIT selected(toolidentificationstring);
    }
  });
  connect(dialogbuttonbox->button(QDialogButtonBox::Reset), &QPushButton::clicked, [this, toolbookmarks]() {
    listwidget->clear();
    for (auto toolbookmark : toolbookmarks) {
      listwidget->addItem(toolbookmark);
    }
  });
  connect(dialogbuttonbox->button(QDialogButtonBox::Ok), &QPushButton::clicked, this, &QDialog::accept);
  connect(dialogbuttonbox->button(QDialogButtonBox::Cancel), &QPushButton::clicked, this, &QDialog::close);
}
