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
#include <QProcess>

class QPushButton;
class ColoredTerminalWidget;


/**
 * @brief      Shows the Output/Status of a tool started as QProcess in a Dialog
 * @ingroup    toolstarter
 */
class ToolProcessDialog : public QDialog {
    Q_OBJECT

  public:
    ToolProcessDialog(const QString& toolname, const QStringList& parameterlist, QWidget *parent = 0, Qt::WindowFlags f = Qt::WindowFlags());
    virtual ~ToolProcessDialog();

  Q_SIGNALS:
    void finishedProcess(int exitCode, QProcess::ExitStatus exitStatus);

  private Q_SLOTS:
    void started();
    void errorOccurred(QProcess::ProcessError error);
    void finished(int exitCode, QProcess::ExitStatus exitStatus);
    void cancel();

    void printStd();
    void printErr();

  private:
    QString errorMsg(QProcess::ProcessError error);

    QPushButton *stopbutton;

    ColoredTerminalWidget *textbox;
    QProcess *process;
};
