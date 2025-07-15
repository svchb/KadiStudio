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

#include <QTextEdit>
class QPushButton;

#include <cpputils/dllapi.hpp>

/**
 * @brief      A terminal-like QTextEdit which supports ANSI color codes
 * @ingroup    enhanced
 */
class DLLAPI ColoredTerminalWidget : public QTextEdit {
  Q_OBJECT

public:
  explicit ColoredTerminalWidget(QWidget* parent = nullptr);

  void setTextTermFormatting(QString const& text, const QTextCharFormat& defaultTextCharFormat = QTextCharFormat());

  void resizeEvent(QResizeEvent *event) Q_DECL_OVERRIDE;
  QSize sizeHint() const Q_DECL_OVERRIDE;

  int getRows() {
    return rows;
  }
  int getColumns() {
    return columns;
  }

private Q_SLOTS:
  void scrollToBottom(void);
  void scrolledTo(int val);

private:
  QColor getLightColor(int colorindex);
  QColor getDarkColor(int colorindex);
  QColor getRGBColor(QListIterator<QString>& i);
  QColor getCubeColor(int index);

  void parseEscapeSequence(int attribute, QListIterator<QString>& i, QTextCharFormat& textCharFormat, QTextCharFormat const& defaultTextCharFormat);

  int rows = 40;
  int columns = 80;

  void posNewContentButton();

  QPushButton *button;
  bool atbottom;

};
