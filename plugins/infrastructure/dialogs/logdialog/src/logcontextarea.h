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

#include <QWidget>

class LogTextWidget;

/**
 * @brief      A side bar which allows to draw additional things next to each text block of a text widget derived from QTextEdit.
 *             See https://stackoverflow.com/a/24596246/3997725
 *             and https://doc.qt.io/qt-5/qtwidgets-widgets-codeeditor-example.html
 * @ingroup    src
 */
class LogContextArea : public QWidget {
    Q_OBJECT

  public:
    explicit LogContextArea(LogTextWidget* logTextWidget);
    QSize sizeHint() const override;

  protected:
    void paintEvent(QPaintEvent* event) override;

  private:
    LogTextWidget *logTextWidget;
};
