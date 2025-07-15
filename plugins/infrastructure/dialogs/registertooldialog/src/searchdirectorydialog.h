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
#include <QVBoxLayout>
#include <QDialogButtonBox>

class SearchDirectoryWidget;

/**
 * @brief      Dialog to add one or several directories to search for executables.
 * @ingroup    tooldialog
 */
class SearchDirectoryDialog : public QDialog {
    Q_OBJECT

  public:
    SearchDirectoryDialog(const QString& pathliststring, QObject *parent);
    virtual ~SearchDirectoryDialog();

    static const char pathseparator;

  protected:
    bool eventFilter(QObject *object, QEvent *event) override;

  Q_SIGNALS:
    void choosenDirectories(const QStringList& directories);

  private Q_SLOTS:
    void addWorkingDirectory();
    void removeWorkingDirectory();
    void dialogReady();
    void accept() override;

  private:
    QList<SearchDirectoryWidget *> searchdirectories;

    QVBoxLayout *dialoglayout;
    QDialogButtonBox *dialogbuttonbox;
    QObject *parent;
    int index;
};
