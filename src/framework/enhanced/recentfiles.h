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

#include <QObject>
#include <QList>
#include <QAction>
#include <QMenu>

#include <cpputils/dllapi.hpp>

/**
 * @brief      Handles the management for recent files
 *
 * Insired by
 * https://www.walletfox.com/course/qtopenrecentfiles.php
 *  @ingroup    framework
 */
class DLLAPI RecentFiles : public QObject {
  Q_OBJECT

  public:
    RecentFiles(QMenu* parentmenu, QString pluginname, int maxrecentfiles = 10);

    QMenu* recentfilesMenu() {
      return recentfilesmenu;
    }

  Q_SIGNALS:

    void openRecentFile(const QString& filename);

  public Q_SLOTS:

    void adjustForCurrentFile(const QString& filepath);

  private Q_SLOTS:

    void clearRecentList();
    void sentRecentFile();

  private:

    void updateRecentActionList();

    QList<QAction*> recentFileActionList;

    QMenu *recentfilesmenu;

    QString pluginname;

};
