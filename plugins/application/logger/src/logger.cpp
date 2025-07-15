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

#include <QScrollBar>
#include <QPointer>

#include "logger.h"


static QPointer<QTextBrowser> logger = NULL;

void myMessageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg) {
  QString format;

  switch (type) {
    case QtDebugMsg:
      format = QString("<div class=\"log debug\">(DD)</div><b> %1</b><br />");
      break;
    case QtWarningMsg:
      format = QString("<div class=\"log warning\">(WW)</div> %1<br />");
      break;
    case QtCriticalMsg:
      format = QString("<div class=\"log critical\">(CC)</div> %1<br />");
      break;
    case QtFatalMsg:
      format = QString("<div class=\"log fatal\">(EE)</div> %1<br />");
      abort();
    default:
      format = QString("<div class=\"log info\">(II)</div> %1<br />");
  }

  logger->insertHtml(format.arg(msg));
}

Logger::Logger() {
  oldHandler = qInstallMessageHandler(myMessageOutput);

  setAcceptRichText(true);
  document()->setDefaultStyleSheet(QString(".log {margin-right:7px; font-weight:bold;} .info {color:#ffffff} .debug {color:#6666ff;} .warning {color:#dddd00;} .critical {color:#990000}"));
  setStyleSheet(QString( "QTextBrowser { background-color:#222; font-family: monospace; color:#ffc; }"));
  setFont(QFont("Courier New", 10));

  connect(this, SIGNAL(textChanged()), this, SLOT(scrollToBottom()));
  connect(this->verticalScrollBar(), SIGNAL(valueChanged(int)), this, SLOT(scrolledTo(int)));

  this->atbottom = true;

  logger = this;
}

Logger::~Logger() {
  qInstallMessageHandler(oldHandler);
  logger = nullptr;
}

void Logger::scrollToBottom(void) {
  if (this->atbottom) {
    verticalScrollBar()->setValue( verticalScrollBar()->maximum() );
  }
}

void Logger::scrolledTo(int val) {
  this->atbottom = (val == verticalScrollBar()->maximum());
}
