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

#include <QtCore/QDir>
#include <QtNetwork>
#include "webviewwidget.h"


WebViewWidget::WebViewWidget(Property* property, QWidget* parent)
    : QPropertyWidget(property, new QWidget(parent)) {

  layout = new QVBoxLayout(getWidget());
  layout->setSpacing(0);
  layout->setContentsMargins(0, 0, 0, 0);

  webView = new QWebEngineView(getWidget());
  webView->setMinimumSize(1750,500);    // TODO: Improve variable size calculation: auto fill parent widget
  layout->addWidget(webView);
  layout->setAlignment(webView, Qt::AlignLeft);

  // setServer(QUrl(QString("127.0.0.1:3333")));

  // Timers
  refreshTimer = new QTimer(getWidget());
  timeoutTimer = new QTimer(getWidget());

  // Connect timers
  connect(refreshTimer, &QTimer::timeout, webView, &QWebEngineView::reload);
  connect(timeoutTimer, &QTimer::timeout, refreshTimer, &QTimer::stop);
  connect(webView, &QWebEngineView::loadFinished, this, &WebViewWidget::handleLoaded);
}

void WebViewWidget::synchronizeVTI() {
  Ambassador *innerAmbassador = getInnerAmbassador();
  if (innerAmbassador) {
    QString url = QString::fromStdString(innerAmbassador->getValue<std::string>("url"));
    if (!url.isEmpty()) {
      loadWebsite(url);
    }
  }
}

void WebViewWidget::setServer(const QUrl& url) {
  server = url;
  if (server.scheme().isEmpty()) {
    server.setScheme(QString::fromStdString("http"));
  }
}

QUrl WebViewWidget::getServer() {
  return server;
}

void WebViewWidget::loadWebsite(const QString& url) {
  // load website
  QUrl serverUrl = QUrl::fromUserInput(url);
  setServer(serverUrl);

  // Load
  webView->load(serverUrl);

  // refresh until website successfully loaded
  timeoutTimer->setSingleShot(true);
  timeoutTimer->start(TIMEOUT);
  refreshTimer->start(REFRESH_INTERVAL);
}

void WebViewWidget::handleLoaded(bool ok) {
  if (ok) {
    refreshTimer->stop();
    timeoutTimer->stop();
  }
}

void WebViewWidget::postRequest(QUrl& url, const QString& cmd) {
  // TODO: add json payload
  QJsonObject postJson = QJsonObject();
  postJson["cmd"] = cmd;
  QJsonDocument doc(postJson);
  QByteArray postData = doc.toJson();

  auto * networkManager = new QNetworkAccessManager(this);
  connect(networkManager, &QNetworkAccessManager::finished, this, &WebViewWidget::onRequestFinished);
  connect(networkManager, &QNetworkAccessManager::finished, networkManager, &QNetworkAccessManager::deleteLater);

  QNetworkRequest request(url);
  request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
  networkManager->post(request, postData);
}

void WebViewWidget::onRequestFinished(QNetworkReply* reply) {
  QByteArray response_data = reply->readAll();
  QString response_message(response_data);

  getInnerAmbassador()->setValue("url",  webView->page()->url().toString().toStdString());
  getInnerAmbassador()->setValue<std::string>("value", "saved");
  emit valueChanged(response_message);
}
