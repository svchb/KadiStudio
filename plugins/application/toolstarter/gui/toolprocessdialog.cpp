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

#include <QMessageBox>
#include <QPushButton>
#include <QVBoxLayout>
#include <framework/enhanced/coloredterminalwidget.h>

#include "toolprocessdialog.h"

ToolProcessDialog::ToolProcessDialog(const QString& toolname, const QStringList& parameterlist, QWidget *parent, Qt::WindowFlags f)
    : QDialog(parent, f) {

  process = new QProcess();

  QStringList filelist = toolname.split("/");
  QString name = filelist.takeLast();
  QString windowTitleString = tr("Output of tool # %1 #").arg(name);
  this->setWindowTitle(windowTitleString);

  QVBoxLayout *layout = new QVBoxLayout;

  textbox = new ColoredTerminalWidget(this);

  QString titleString = tr("Execution String: %1 %2").arg(toolname, parameterlist.join(" "));
  textbox->setTextColor(QColor(Qt::blue));
  textbox->append(titleString);
  layout->addWidget(textbox);

  stopbutton = new QPushButton(tr("Stop Process"), this);
  layout->addWidget(stopbutton);

  setLayout(layout);

  connect(stopbutton, SIGNAL(clicked()), this, SLOT(cancel()));

  connect(process, SIGNAL(started()), this, SLOT(started()));
  connect(process, SIGNAL(errorOccurred(QProcess::ProcessError)), this, SLOT(errorOccurred(QProcess::ProcessError)));
  connect(process, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(finished(int, QProcess::ExitStatus)));
  connect(process, SIGNAL(readyReadStandardOutput()), this, SLOT(printStd()));
  connect(process, SIGNAL(readyReadStandardError()), this, SLOT(printErr()));

  process->setProcessChannelMode(QProcess::MergedChannels);

  QString executable_name = toolname;
  QStringList arguments = parameterlist;

  // the toolname can contain arguments (e.g. "kadi-apy records create"), which is not understood by QProcess
  if (executable_name.contains(" ")) {
    QStringList toolname_segments = toolname.split(" ", Qt::SkipEmptyParts);
    executable_name = toolname_segments.at(0);
    toolname_segments.removeFirst();
    for (int index = toolname_segments.size() - 1; index >= 0; index--) {
      const QString& toolname_segment = toolname_segments.at(index);
      arguments.push_front(toolname_segment);
    }
  }

  QStringList environment = QProcess::systemEnvironment();
  environment.append("LINES=" + QString::number(textbox->getRows()));
  environment.append("COLUMNS=" + QString::number(textbox->getColumns()));
  process->setEnvironment(environment);

  process->start(executable_name, arguments, QIODevice::Unbuffered | QProcess::ReadWrite);
  process->waitForStarted();
}

ToolProcessDialog::~ToolProcessDialog() {
}

void ToolProcessDialog::started() {
  textbox->setTextColor(QColor(Qt::blue));
  textbox->append(tr("Process started with PID %1").arg(QString::number(process->processId())));
  textbox->setTextColor(QColor(Qt::black));
  textbox->append("\n");

  this->setWindowTitle(windowTitle().append(QString(" (PID: %2)").arg(process->processId())));
}

void ToolProcessDialog::cancel() {
  stopbutton->setDisabled(true);
  disconnect(process, SIGNAL(errorOccurred(QProcess::ProcessError)), this, SLOT(errorOccurred(QProcess::ProcessError)));
  disconnect(process, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(finished(int, QProcess::ExitStatus)));
  process->terminate();
  textbox->setTextColor(QColor(Qt::red));
  textbox->append("\n" + tr("Canceled by user!") + "\n");

  stopbutton->setText(tr("Canceled by user!"));
}

QString ToolProcessDialog::errorMsg(QProcess::ProcessError error) {
  switch (error) {
    case QProcess::FailedToStart: return tr("failed to start"); break;
    case QProcess::Crashed:       return tr("crashed");         break;
    case QProcess::Timedout:      return tr("timedout");        break;
    case QProcess::WriteError:    return tr("write error");     break;
    case QProcess::ReadError:     return tr("read error");      break;
    default:                      return tr("unknown error");   break; // QProcess::UnknownError
  }
}

void ToolProcessDialog::errorOccurred(QProcess::ProcessError error) {
  textbox->setTextColor(QColor(Qt::red));
  textbox->append("\n" + tr("An error occured: %1").arg(errorMsg(error)) + "\n");
  stopbutton->setText(tr("An error occured: %1").arg(errorMsg(error)));
  QMessageBox::warning(this, "ToolStarter", tr("An error occured: %1").arg(errorMsg(error)));
}

void ToolProcessDialog::finished(int exitCode, QProcess::ExitStatus exitStatus) {
  stopbutton->setDisabled(true);
  if (exitStatus == QProcess::NormalExit) {
    if (exitCode == 0) {
      textbox->setTextColor(QColor(Qt::blue));
      textbox->append("\n" + tr("Process finished successfully!") + "\n");
      stopbutton->setText(tr("Process finished successfully!"));
      QMessageBox::information(this, "ToolStarter", tr("Process finished successfully!"));
    } else {
      textbox->setTextColor(QColor(Qt::red));
      textbox->append("\n" + tr("Process finished with exitcode: %1").arg(QString::number(exitCode)) + "\n");
      stopbutton->setText(tr("Process finished with exitcode: %1").arg(QString::number(exitCode)));
      QMessageBox::warning(this, "ToolStarter", tr("Process finished with exitcode: %1").arg(QString::number(exitCode)));
    }
  }

  Q_EMIT finishedProcess(exitCode, exitStatus);
}

void ToolProcessDialog::printStd() {
  // textbox->setTextColor(QColor(Qt::black));
  // textbox->append(process->readAllStandardOutput());
  textbox->setTextTermFormatting(process->readAllStandardOutput());
}

void ToolProcessDialog::printErr() {
  // textbox->setTextColor(QColor(Qt::red));
  // textbox->append(process->readAllStandardError());
  textbox->setTextTermFormatting(process->readAllStandardError());
}
