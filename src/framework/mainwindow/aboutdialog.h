/* Copyright 2022 Karlsruhe Institute of Technology
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

class QLabel;
class QListWidget;
class QListWidgetItem;


/**
 * @brief      The about dialog widget which displays useful information
 *             about the institute and license of Kadi studio.
 * @ingroup    framework
 */
class AboutStudioDialog : public QDialog {
  Q_OBJECT

  public:
    AboutStudioDialog(const QString& callernamespace = "", QWidget* parent = nullptr);

  private:
    QListWidget *listwidget;
    QLabel *basicInformationLbl;

    void setAboutText();
    void setPluginDescription(QListWidgetItem *selected);

    int changed = 0; ///< multi state for mouse and key press order (might break in future qt versions)

    QListWidgetItem *previousselected;

  private Q_SLOTS:
    void setSelected(QListWidgetItem *selected);
    void itemSelectionChanged();
    void currentRowChanged(int currentRow);
    void itemClicked(QListWidgetItem* item);

};
