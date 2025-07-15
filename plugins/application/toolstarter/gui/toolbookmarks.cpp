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

#include <QDebug>
#include <QLineEdit>
#include <QApplication>
#include <QMenu>
#include <QSettings>

#include "toolstarterwidget.h"
#include "toolbookmarksdialog.h"
#include "toolbookmarks.h"

ToolBookmarks::ToolBookmarks(ToolStarterWidget* tsw, QMenu* menu, QLineEdit* toolstring_widget) : tsw(tsw), menu(menu) {
  settings = new QSettings(qApp->applicationName(), "/plugins/application/toolstarter");

  bookmarkcount = settings->beginReadArray("bookmarks");
  for (int i = 0; i < bookmarkcount; i++) {
    settings->setArrayIndex(i);
    QString toolstring = settings->value("toolstring").toString();
    QAction *bookmarkaction = addBookmark(toolstring);
    if (i < 10) {
      bookmarkaction->setShortcut(QKeySequence("Ctrl+"+QString::number(i+1)));
    }
    menu->addAction(bookmarkaction);
  }
  settings->endArray();

  separator = menu->addSeparator();

  QAction *addaction = new QAction(tr("Bookmark actual configuration"), menu);
  addaction->setStatusTip(tr("Remember the actual configuration for later use"));
  addaction->setShortcut(QKeySequence("Ctrl+B"));
  connect(addaction, &QAction::triggered, [this, toolstring_widget]() {
    QString toolstring = toolstring_widget->text();
    if (toolstring.isEmpty()) return;
    QAction *bookmarkaction = addBookmark(toolstring);
    if (bookmarkcount < 10) {
      bookmarkaction->setShortcut(QKeySequence("Ctrl+"+QString::number(bookmarkcount)));
    }
    this->menu->insertAction(separator, bookmarkaction);
    settings->beginWriteArray("bookmarks");
    settings->setArrayIndex(bookmarkcount++);
    settings->setValue("toolstring", toolstring);
    settings->endArray();
  });
  menu->addAction(addaction);

  QAction *managebookmarkaction = new QAction(tr("Manage Bookmarks..."), menu);
  managebookmarkaction->setStatusTip(tr("Manage Bookmarks"));
  managebookmarkaction->setShortcut(QKeySequence("Ctrl+M"));
  connect(managebookmarkaction, &QAction::triggered, [this]() {
    execBookmarkDialog();
  });
  menu->addAction(managebookmarkaction);
}

QAction* ToolBookmarks::addBookmark(QString toolstring) {
  QAction *entry = new QAction(toolstring, menu);
  connect(entry, &QAction::triggered, [this, entry]() {
    QString toolidentificationstring = entry->text();
    qDebug() << toolidentificationstring;
    tsw->toolChanged(toolidentificationstring);
  });
  return entry;
}

QList<QString> ToolBookmarks::getBookmarks() {
  QList<QString> toolbookmarks;
  for (auto action : menu->actions()) {
    if (action == separator) break;
    toolbookmarks << action->text();
  }
  return toolbookmarks;
}

void ToolBookmarks::execBookmarkDialog() {
  QList<QString> toolbookmarks = getBookmarks();

  ToolBookmarksDialog dialog(toolbookmarks, tsw);

  connect(&dialog, &ToolBookmarksDialog::selected, tsw, &ToolStarterWidget::toolChanged);

  if (dialog.exec() == QDialog::Accepted) {
    for (auto action : menu->actions()) {
      if (action == separator) break;
      menu->removeAction(action);
    }
    settings->remove("bookmarks");
    settings->beginWriteArray("bookmarks");
    toolbookmarks = dialog.getBookmarks();
    for (int i = 0; i < toolbookmarks.count(); i++) {
      settings->setArrayIndex(i);
      QString toolstring = toolbookmarks.at(i);
      this->menu->insertAction(separator, addBookmark(toolstring));

      settings->setValue("toolstring", toolstring);

    }
    settings->endArray();
  }
}
