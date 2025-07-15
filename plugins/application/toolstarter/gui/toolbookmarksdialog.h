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

#pragma once

#include <QDialog>
#include <QListWidget>


/**
 * @brief      Dialog to andle bookmarks to favorit executions profiles of tools.
 * @ingroup    toolstarter
 */
class ToolBookmarksDialog : public QDialog {
  Q_OBJECT

public:
  ToolBookmarksDialog(QList<QString>& toolbookmarks, QWidget* parent = nullptr);
  ~ToolBookmarksDialog() = default;

  QList<QString> getBookmarks() {
    QList<QString> toolbookmarks;
    for (int i = 0; i < listwidget->count(); i++) {
      QListWidgetItem *item = listwidget->item(i);
      QString toolstring = item->text();
      toolbookmarks << toolstring;
    }
    return toolbookmarks;
  }

Q_SIGNALS:
  void selected(QString toolidentificationstring);

private:

  QListWidget *listwidget;

};
