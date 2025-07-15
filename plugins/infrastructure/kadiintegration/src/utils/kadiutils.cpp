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

#include <QEventLoop>
#include <QTimer>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>

#include <QJsonDocument>
#include <QJsonObject>

#include <QRegularExpression>
#include <QRegularExpressionMatch>

#include <QWidget>
#include <QString>
#include <QProcess>
#include <QMessageBox>
#include <QNetworkReply>
#include <QNetworkRequest>

#include "kadiutils.h"


void kadiutils::yieldForMilliseconds(int milliseconds) {
  QEventLoop loop;
  QTimer t;
  QTimer::connect(&t, &QTimer::timeout, &loop, &QEventLoop::quit);
  t.start(milliseconds);
  loop.exec();
}

void kadiutils::showWarningWithProcOutput(QWidget* parent, const QString& title, const QString& text, QProcess& qProcess) {
  QMessageBox::warning(parent, title, text + "\n\nstdout:\n"
                                      + qProcess.readAllStandardOutput() + "\n\nstderr:\n" + qProcess.readAllStandardError());
}

bool kadiutils::parseJsonData(QNetworkReply* reply, QJsonObject& jsonobject) {
  QByteArray s = reply->readAll();
  QJsonParseError jsonerror;
  QJsonDocument jsondoc(QJsonDocument::fromJson(s, &jsonerror));
  if (jsonerror.error != QJsonParseError::NoError) {
    QMessageBox mbox;
    mbox.setIcon(QMessageBox::Warning);
    qWarning() << "ERROR: Parsing json data: " << jsonerror.errorString() << " from " << reply->url().toString();
    mbox.setText(QObject::tr("Error while parsing JSON from") + reply->url().toString());
    mbox.setInformativeText(jsonerror.errorString() + " at position " + QString::number(jsonerror.offset));
    mbox.adjustSize();
    mbox.exec();
    return false;
  }
  jsonobject = jsondoc.object();
  return true;
}

bool kadiutils::getRecordIdentifier(const QString &filepath, QString& kadiInstanceName, QString& recordIdentifier, QString& kadifilename) {
  if (not filepath.isEmpty()) {
    QRegularExpression regex_withfilename("kadi://(.*?)/(.*?)/(.*)");
    QRegularExpressionMatch match_withfilename = regex_withfilename.match(filepath);
    if (match_withfilename.hasMatch()) {
      kadiInstanceName = match_withfilename.captured(1);
      recordIdentifier = match_withfilename.captured(2);
      kadifilename     = match_withfilename.captured(3);
      return true;
    }
  }
  return false;
}

int kadiutils::getHttpCode(QNetworkReply* networkReply) {
  return networkReply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
}

void kadiutils::reportErrorToUser(QNetworkReply* networkReply, QWidget* parent) {
  // show a message box in case of an error with the response from the api for easy reasoning
  QMessageBox mbox(parent);
  mbox.setIcon(QMessageBox::Warning);
  mbox.setText(QObject::tr("Error while placing request: HTTP Code ") + QString::number(getHttpCode(networkReply)));

  // parse json
  QByteArray s = networkReply->readAll();
  QJsonDocument jsondoc(QJsonDocument::fromJson(s));
  QJsonObject jsonobject = jsondoc.object();

  QString msg = jsonobject["description"].toString();
  if (msg.isEmpty()) msg = QObject::tr("No error message provided.");
  mbox.setInformativeText(msg);

  mbox.adjustSize();
  mbox.exec();
}

void kadiutils::NetworkHandler::sendRequest(const QNetworkRequest& request) {
  qDebug() << request.url();
  qDebug() << request.rawHeaderList();
  auto *reply = networkAccessManager->get(request);

  connect(reply, &QNetworkReply::finished, this, [this, reply] {
    handleRequest(reply);
    reply->deleteLater();
  });
}

void kadiutils::NetworkHandler::handleRequest(QNetworkReply* reply) {
  int statuscode = getHttpCode(reply);

  // handle 429 error
  if (statuscode == 429) {
    qWarning() << "429 received, delaying retry..." << Qt::endl;
    kadiutils::yieldForMilliseconds(500);

    QNetworkRequest request(reply->request());

    auto *retryReply = networkAccessManager->get(request);

    connect(retryReply, &QNetworkReply::finished, this, [this, retryReply] {
      handleRequest(retryReply);
      retryReply->deleteLater();
    });

    return;
  }

  if (reply->error()) reportErrorToUser(reply, parent);

  Q_EMIT finished(reply);
}
