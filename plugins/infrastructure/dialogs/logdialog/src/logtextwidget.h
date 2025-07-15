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
#include <framework/enhanced/coloredterminalwidget.h>

class LogContent;

/**
 * @brief      This widget extends a text widget with a area on the left showing a colored rectangle
 *             with a color unique to the context.
 *             See https://stackoverflow.com/a/24596246/3997725
 *             and https://doc.qt.io/qt-5/qtwidgets-widgets-codeeditor-example.html
 * @ingroup    src
 */
class LogTextWidget : public ColoredTerminalWidget {
  Q_OBJECT

  public:
    explicit LogTextWidget(QWidget *parent = nullptr);

    void logContextAreaPaintEvent(QPaintEvent *event);
    int getLogContextAreaWidth() const;
    void setCurrentContent(const LogContent* context);

  protected:
    void resizeEvent(QResizeEvent* event) override;

  private Q_SLOTS:
    void updateLogContextAreaWidth(int block_count);
    void updateLogContextArea();

  private:
    int getFirstVisibleBlockId();
    static QColor stringToColor(const std::string& str);
    static void adjustBrightness(QColor& color, int minBrightness);

    QWidget *log_context_area;
    const LogContent *current_content;
};
