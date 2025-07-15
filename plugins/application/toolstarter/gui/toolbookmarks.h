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

#include <QWidget>

class QMenu;
class QSettings;
class QLineEdit;
class ToolStarterWidget;

/**
 * @brief      Handle bookmarks to favorit executions profiles of tools.
 * @ingroup    toolstarter
 */
class ToolBookmarks : public QObject {
    Q_OBJECT

public:

  ToolBookmarks(ToolStarterWidget* tsw, QMenu* menu, QLineEdit* toolstring_widget);

private:

  QAction* addBookmark(QString toolstring);
  QList<QString> getBookmarks();
  void execBookmarkDialog();

  int    bookmarkcount;

  ToolStarterWidget *tsw;

  QMenu *menu;
  QAction *separator;
  QSettings *settings;

};
