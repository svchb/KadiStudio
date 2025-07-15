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

#include <QPaintEvent>
#include <QPainter>
#include <QScrollBar>
#include <QTextDocument>
#include <QTextBlock>
#include <QAbstractTextDocumentLayout>

#include "logcontent.h"
#include "logtextwidget.h"
#include "logcontextarea.h"

LogTextWidget::LogTextWidget(QWidget* parent)
    : ColoredTerminalWidget(parent), current_content(nullptr) {
  log_context_area = new LogContextArea(this);

  connect(document(), &QTextDocument::blockCountChanged, this, &LogTextWidget::updateLogContextArea);
  connect(this, &QTextEdit::textChanged, this, &LogTextWidget::updateLogContextArea);
  connect(verticalScrollBar(), &QScrollBar::valueChanged, this, &LogTextWidget::updateLogContextArea);
  connect(this, &QTextEdit::textChanged, this, &LogTextWidget::updateLogContextArea);

  updateLogContextAreaWidth(0);
}

int LogTextWidget::getLogContextAreaWidth() const {
  return 20;
}

void LogTextWidget::updateLogContextArea() {
  // verticalScrollBar()->setSliderPosition(verticalScrollBar()->sliderPosition());

  QRect rect =  contentsRect();
  log_context_area->update(0, rect.y(), log_context_area->width(), rect.height());
  updateLogContextAreaWidth(0);
  int dy = verticalScrollBar()->sliderPosition();
  if (dy > -1) {
    log_context_area->scroll(0, dy);
  }
  int first_block_id = getFirstVisibleBlockId();
  if (first_block_id == 0 || textCursor().block().blockNumber() == first_block_id - 1) {
    verticalScrollBar()->setSliderPosition(dy - document()->documentMargin());
  }
}

void LogTextWidget::resizeEvent(QResizeEvent* event) {
  ColoredTerminalWidget::resizeEvent(event);

  QRect rect = contentsRect();
  log_context_area->setGeometry(QRect(rect.left(), rect.top(), getLogContextAreaWidth(), rect.height()));
}

int LogTextWidget::getFirstVisibleBlockId() {
  QTextCursor cursor = QTextCursor(document());
  cursor.movePosition(QTextCursor::Start);
  for (int i = 0; i < document()->blockCount(); ++i) {
    QTextBlock block = cursor.block();

    QRect rect1 = viewport()->geometry();
    QRect rect2 = document()->documentLayout()->blockBoundingRect(block).translated(
            viewport()->geometry().x(), viewport()->geometry().y() - (
                    verticalScrollBar()->sliderPosition()
            )).toRect();

    if (rect1.contains(rect2, true)) {
      return i;
    }

    cursor.movePosition(QTextCursor::NextBlock);
  }

  return 0;
}

void LogTextWidget::logContextAreaPaintEvent(QPaintEvent* event) {
  verticalScrollBar()->setSliderPosition(verticalScrollBar()->sliderPosition());

  if (!current_content) return;

  QPainter painter(log_context_area);
  int block_number = std::max(getFirstVisibleBlockId(), 0);

  QTextBlock block = document()->findBlockByNumber(block_number);
  QTextBlock prev_block = (block_number > 0) ? document()->findBlockByNumber(block_number-1) : block;
  int translate_y;
  if (block_number > 0) {
    translate_y = - verticalScrollBar()->sliderPosition();
  } else {
    translate_y = 0;
  }

  int top = viewport()->geometry().top();

  // adjust drawing position according to the previous "non entirely visible" block
  // if applicable. Also takes in consideration the document's margin offset.
  int additional_margin;
  if (block_number == 0) {
    // simply adjust to document's margin
    additional_margin = (int) document()->documentMargin() - 1 - verticalScrollBar()->sliderPosition();
  } else {
    // getting the height of the visible part of the previous "non entirely visible" block
    additional_margin = (int) document()->documentLayout()->blockBoundingRect(prev_block)
            .translated(0, translate_y).intersected(viewport()->geometry()).height();
  }

  // shift the starting point
  top += additional_margin;

  int block_height = (int) document()->documentLayout()->blockBoundingRect(block).height();
  int bottom = top + block_height;

  while (block.isValid() && top <= event->rect().bottom()) {
    if (block.isVisible() && bottom >= event->rect().top()) {
      QString number = QString::number(block_number + 1);
      std::string context = current_content->getContextForLine(block_number);
      if (!context.empty()) {
        QBrush brush(stringToColor(context));
        painter.setBrush(brush);
        painter.setPen(Qt::NoPen);
        painter.drawRect(-5, top, log_context_area->width(), block_height);
      }
    }

    block = block.next();
    top = bottom;
    bottom = top + (int) document()->documentLayout()->blockBoundingRect(block).height();
    ++block_number;
  }
}

void LogTextWidget::updateLogContextAreaWidth(int /*block_count*/) {
  setViewportMargins(getLogContextAreaWidth(), 0, 0, 0);
}

void LogTextWidget::setCurrentContent(const LogContent* content) {
  current_content = content;
}

void LogTextWidget::adjustBrightness(QColor& color, int minBrightness) {
  double brightness = (0.299 * color.redF() + 0.587 * color.greenF() + 0.114 * color.blueF()) / 255.0;

  if (brightness < (minBrightness / 255.0)) {
    double factor = (minBrightness / 255.0) / brightness;
    color.setRed(std::min((unsigned int) (color.redF() * factor), 255u));
    color.setGreen(std::min((unsigned int) (color.greenF() * factor), 255u));
    color.setBlue(std::min((unsigned int) (color.blueF() * factor), 255u));
  }
}

QColor LogTextWidget::stringToColor(const std::string& str) {
  std::hash<std::string> hasher;
  auto hash = hasher(str);

  int r = (hash & 0xFF0000) >> 16;
  int g = (hash & 0x00FF00) >> 8;
  int b = hash & 0x0000FF;

  QColor color(r, g, b);
  adjustBrightness(color, 150);
  return color;
}
