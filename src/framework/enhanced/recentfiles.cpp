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

#include <QApplication>
#include <QSettings>
#include <QUrl>
#include <QFileInfo>

#include "recentfiles.h"


RecentFiles::RecentFiles(QMenu* parentmenu, QString pluginname, int maxrecentfiles) : pluginname(pluginname) {
  // create actions and connections
  recentfilesmenu = parentmenu->addMenu(tr("Open Recent"));
  recentfilesmenu->setToolTipsVisible(true);

  for (auto i = 0; i < maxrecentfiles; ++i) {
    QAction *recentFileAction = new QAction();
    recentFileAction->setVisible(false);
    connect(recentFileAction, SIGNAL(triggered()), this, SLOT(sentRecentFile()));
    recentFileActionList.append(recentFileAction);
    recentfilesmenu->addAction(recentFileAction);
  }

  recentfilesmenu->addSeparator();

  QAction *clearAction = recentfilesmenu->addAction(tr("Clear List"));
  connect(clearAction, SIGNAL(triggered()), this, SLOT(clearRecentList()));

  updateRecentActionList();
}

void RecentFiles::updateRecentActionList() {
  QSettings settings(qApp->applicationName(), pluginname);
  QStringList recentfilepaths = settings.value("recentFiles").toStringList();

  if (recentfilepaths.size() == 0) {
    recentfilesmenu->setEnabled(false);
    return;
  }

  recentfilesmenu->setEnabled(true);

  auto itEnd = (recentfilepaths.size() < recentFileActionList.size()) ? recentfilepaths.size() : recentFileActionList.size();

  for (auto i = 0; i < itEnd; ++i) {
    QAction *recentFileAction = recentFileActionList.at(i);
    QUrl recentfilepath(recentfilepaths.at(i));
    recentFileAction->setText(recentfilepath.fileName());
    recentFileAction->setToolTip(recentfilepath.adjusted(QUrl::RemoveFilename).toString());
    recentFileAction->setData(recentfilepath.toString());
    recentFileAction->setVisible(true);
  }

  for (auto i = itEnd; i < recentFileActionList.size(); ++i) {
    recentFileActionList.at(i)->setVisible(false);
  }
}

void RecentFiles::adjustForCurrentFile(const QString& filepath) {
  if (filepath.isEmpty()) {
    return;
  }

  QSettings settings(qApp->applicationName(), pluginname);
  QStringList recentfilepaths = settings.value("recentFiles").toStringList();

  recentfilepaths.removeAll(filepath);
  recentfilepaths.prepend(filepath);
  while (recentfilepaths.size() > recentFileActionList.size()) {
    recentfilepaths.removeLast();
  }
  settings.setValue("recentFiles", recentfilepaths);

  updateRecentActionList();
}

void RecentFiles::sentRecentFile() {
  QAction *action = qobject_cast<QAction *>(sender());
  if (action) {
    QString filepath = action->data().toString();
    Q_EMIT openRecentFile(filepath);
  }
}

void RecentFiles::clearRecentList() {
  QSettings settings(qApp->applicationName(), pluginname);
  settings.setValue("recentFiles", {});

  updateRecentActionList();
}
