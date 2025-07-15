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

class QWidget;
class QString;
class QProcess;
class QNetworkAccessManager;
class QNetworkReply;
class QNetworkRequest;


/**
 * @file       kadiutils.h
 * @brief      Provides means to integrate Kadi by providing utility
 *             functions to access kadi-apy and kadi web directly.
 * @ingroup    kadiintegration
 */

namespace kadiutils {

  /**
   * @brief Allows pending events in the event loop to be processed
   * See also:
   *    - https://stackoverflow.com/a/43003223
   *    - https://doc.qt.io/archives/qq/qq27-responsive-guis.html#waitinginalocaleventloop
   * @param milliseconds how long to allow pending events to pe processed
   */
  void yieldForMilliseconds(int milliseconds);

  /**
   * @brief Shows a message box with the output of the process nicely formatted.
   * @param parent optional parent of the QMessageBox
   * @param title title of the message box
   * @param text informative text to diplay above the formatted output
   * @param qProcess process to get the output from
   */
  void showWarningWithProcOutput(QWidget* parent, const QString &title, const QString &text, QProcess &qProcess);

  /**
   * @brief Wraps parsing and error handling into one function.
   * @param reply containing the replay from a request
   * @param jsonobject the parsed json object
   * @return true if the parsed string is a valid JSON
   */
  bool parseJsonData(QNetworkReply* reply, QJsonObject& jsonobject);


  bool getRecordIdentifier(const QString &filepath, QString& kadiInstanceName, QString& recordIdentifier, QString& kadifilename);

  /**
   * @brief Extracts the HTTP status from the provided reply.
   * @param networkReply reply to extract from
   * @return the HTTP status code of that reply
   */
  int getHttpCode(QNetworkReply *networkReply);

  /**
   * Checks the supplied QNetworkReply for errors, shows a message box if errors occurred.
   * @param networkReply reply to check
   */
  void reportErrorToUser(QNetworkReply* networkReply, QWidget* parent);

class NetworkHandler : public QObject {
  Q_OBJECT

public:
  NetworkHandler(QNetworkAccessManager* networkAccessManager, QWidget* parent = nullptr)
      : networkAccessManager(networkAccessManager),
        parent(parent) {
  }

  ~NetworkHandler() = default;

  void sendRequest(const QNetworkRequest& request);

Q_SIGNALS:
  void finished(QNetworkReply* reply);

private:

  /**
   * Checks the http code and if not status code 429 send a signal
   * @param networkReply reply to handle
   */
  void handleRequest(QNetworkReply* reply);

  QNetworkAccessManager *networkAccessManager;

  QWidget *parent;

};

}
