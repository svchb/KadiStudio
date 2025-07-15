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

#include <QUrl>
#include <QTimer>
#include <QtWidgets/QWidget>
#include <QtWebEngineWidgets/QWebEngineView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QPushButton>
#include <properties/ui/factory/propertywidget.h>
#include "../../../qpropertywidget.h"


/**
 * @brief      A widget to display and interact with websites
 * @ingroup    qtpropertywidgetfactory
 */
class WebViewWidget : public QPropertyWidget {
  Q_OBJECT

  public:
    WebViewWidget(Property* property, QWidget* parent = nullptr);

    void synchronizeVTI() override;

    QUrl getServer();
    void setServer(const QUrl& url);

    void loadWebsite(const QString& url);
    void postRequest(QUrl& url, const QString& cmd);

  public
    Q_SLOTS:
    void handleLoaded(bool ok);
    void onRequestFinished(QNetworkReply *rep);

    Q_SIGNALS:
    void valueChanged(QString value);

  private:
    QVBoxLayout *layout;
    QWebEngineView *webView;
    QPushButton *saveButton;  ///< button to serialize website to json
    QUrl server;             ///< server for http POST and GET requests
    QTimer *refreshTimer;
    QTimer *timeoutTimer;
    const int REFRESH_INTERVAL = 3000;
    const int TIMEOUT = 30000;
};
