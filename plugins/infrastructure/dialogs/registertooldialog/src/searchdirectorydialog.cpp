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

#include <QMouseEvent>

#include "searchdirectorydialog.h"
#include "searchdirectorywidget.h"


#ifdef Q_OS_WIN
const char SearchDirectoryDialog::pathseparator = ';';
#else
const char SearchDirectoryDialog::pathseparator = ':';
#endif

SearchDirectoryDialog::SearchDirectoryDialog(const QString& pathliststring, QObject *parent)
    : parent(parent),
      index(0) {

  setWindowTitle(tr("Search directories for tools"));

  dialoglayout = new QVBoxLayout;

  dialogbuttonbox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
  connect(dialogbuttonbox, SIGNAL(accepted()), this, SLOT(accept()));
  connect(dialogbuttonbox, SIGNAL(rejected()), this, SLOT(close()));

  dialoglayout->addWidget(dialogbuttonbox);

  QStringList pathlist = pathliststring.split(pathseparator);

  if (pathlist.size() > 0) {
    for (index=0; index<pathlist.size(); index++) {
      addWorkingDirectory();
      SearchDirectoryWidget *searchdirectory = searchdirectories.last();
      searchdirectory->setText(pathlist[index]);
      searchdirectory->lineEdit()->clearFocus();
    }
    if (pathlist.size() == 1) searchdirectories.first()->minusButton()->setDisabled(true);
  } else {
    addWorkingDirectory();
  }
  dialogReady();

  setLayout(dialoglayout);
}

SearchDirectoryDialog::~SearchDirectoryDialog() {
}

void SearchDirectoryDialog::accept() {
  QStringList directories;
  Q_FOREACH (SearchDirectoryWidget *searchdirectory, searchdirectories) {
    QString directory = searchdirectory->lineEdit()->text();
    directories << directory;
  }
  Q_EMIT choosenDirectories(directories);
  QDialog::accept();
}

void SearchDirectoryDialog::addWorkingDirectory() {
  SearchDirectoryWidget *searchdirectory = new SearchDirectoryWidget(this);
  QPushButton *plusbutton = searchdirectory->plusButton();
  QPushButton *minusbutton = searchdirectory->minusButton();
  QLineEdit *lineedit = searchdirectory->lineEdit();
  plusbutton->installEventFilter(this);
  minusbutton->installEventFilter(this);
  connect(plusbutton, SIGNAL(clicked()), this, SLOT(addWorkingDirectory()));
  connect(minusbutton, SIGNAL(clicked()), this, SLOT(removeWorkingDirectory()));
  connect(lineedit, SIGNAL(textChanged(QString)), this, SLOT(dialogReady()));
  searchdirectories.insert(index, searchdirectory);
  dialoglayout->insertWidget(index, searchdirectory);
  dialogbuttonbox->button(QDialogButtonBox::Ok)->setDisabled(true);

  if (searchdirectories.size() > 1) {
    searchdirectories.first()->minusButton()->setDisabled(false);
  }
}

void  SearchDirectoryDialog::removeWorkingDirectory() {
  SearchDirectoryWidget *searchdirectory = searchdirectories.takeAt(index);
  dialoglayout->removeWidget(searchdirectory);
  delete searchdirectory;
  if (searchdirectories.size() == 1) {
    searchdirectories.first()->minusButton()->setDisabled(true);
  }
  adjustSize();
  dialogReady();
}

bool SearchDirectoryDialog::eventFilter(QObject *object, QEvent *event) {
  if (event->type() == QEvent::MouseButtonPress) {
    QPushButton *button = dynamic_cast<QPushButton *>(object);
    if (button) {
      Q_FOREACH (SearchDirectoryWidget * searchdirectory, searchdirectories) {
        QPushButton *plusbutton = searchdirectory->plusButton();
        QPushButton *minusbutton = searchdirectory->minusButton();

        if (button == plusbutton) {
          index = searchdirectories.indexOf(searchdirectory) + 1;
          break;
        } else if (button == minusbutton) {
          index = searchdirectories.indexOf(searchdirectory);
          break;
        }
      }
    }
  }

  return parent->eventFilter(object, event);
}

void SearchDirectoryDialog::dialogReady() {
  bool ready = true;
  Q_FOREACH (SearchDirectoryWidget * searchdirectory, searchdirectories) {
    QString directory = searchdirectory->lineEdit()->text();

    if (directory.isEmpty()) {
      ready = false;
      break;
    }
  }
  dialogbuttonbox->button(QDialogButtonBox::Ok)->setEnabled(ready);
}
