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
#include <QDateTime>

class QToolBox;
class QTextEdit;


/**
 * @brief      Shows recent usage of the currently selected tool.
 * @ingroup    toolstarter
 */
class ToolHistoryDialog : public QDialog {
    Q_OBJECT

  public:
    ToolHistoryDialog();
    virtual ~ToolHistoryDialog();

    void addToolToHistory(const QString& toolidentificationstring, const QDateTime& datetime);

  Q_SIGNALS:
    void toolReset();
    void toolChanged(QString toolxml);

  private Q_SLOTS:
    void changeTool();

  private:
    void loadHistory();
    void addTool(const QString& toolidentificationstring, const QDateTime& datetime);
    void showHelpText();

    QToolBox *toolboxwidget;
    QTextEdit *textedit;
    QStringList toolidentificationstrings;

};
