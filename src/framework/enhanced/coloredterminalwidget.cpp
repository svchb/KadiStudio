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

#include <QDebug>
#include <QFont>
#include <QTimeLine>
#include <QPushButton>
#include <QStyle>
#include <QGridLayout>
#include <QLabel>
#include <QScrollBar>
#include <QFontDatabase>
#include <QRegularExpression>

#include "coloredterminalwidget.h"

ColoredTerminalWidget::ColoredTerminalWidget(QWidget* parent) : QTextEdit(parent) {
  setFont(QFont("monospace", 12));
  // setStyleSheet("background-color: #2b2d2f; color: white");

  setReadOnly(true);

  button = new QPushButton(this);  // parent is current QWidget

  button->setIcon(this->style()->standardIcon(QStyle::SP_ArrowDown));
  button->setText(tr("New Content"));
  button->setStyleSheet("text-align:left;");

  button->setStyleSheet("font: bold; outline:10px black; color: rgba(255,255,255,155); background-color: rgba(10,10,255,100)");

  button->show(); // do recalculation of size hints
  button->hide();

  connect(button, &QPushButton::pressed, this, [this]() {
    verticalScrollBar()->setValue( verticalScrollBar()->maximum() );
  });

  connect(this, SIGNAL(textChanged()), this, SLOT(scrollToBottom()));
  connect(this->verticalScrollBar(), SIGNAL(valueChanged(int)), this, SLOT(scrolledTo(int)));

  this->atbottom = true;
}

QSize ColoredTerminalWidget::sizeHint() const {
  const QFont &font = this->document()->defaultFont();
  const QFontMetrics fontmetrics(font);
  int width = fontmetrics.maxWidth()*columns;
  int height = fontmetrics.lineSpacing()*rows;
  return QSize(width, height);
}

void ColoredTerminalWidget::posNewContentButton() {
  const int margin = 20;  // arbitrary margin
  QSize btnSize = button->frameSize();
  QSize containerSize = this->size();
  QSize scrollbarSize = verticalScrollBar()->frameSize();
  int newX = containerSize.width()  - (btnSize.width()  + scrollbarSize.width() + margin);
  int newY = containerSize.height() - (btnSize.height() + margin);
  // position the button
  button->move(newX, newY);
}

void ColoredTerminalWidget::resizeEvent(QResizeEvent *event) {
  posNewContentButton();
  // call the superclass handler
  QTextEdit::resizeEvent(event);
}

QColor ColoredTerminalWidget::getLightColor(int colorindex) {
  switch (colorindex) {
    case 0 : return Qt::darkGray;
    case 1 : return Qt::red;
    case 2 : return Qt::green;
    case 3 : return Qt::yellow;
    case 4 : return Qt::blue;
    case 5 : return Qt::magenta;
    case 6 : return Qt::cyan;
    case 7 : return Qt::white;
    default : Q_ASSERT(false); return QColor();
  }
}

QColor ColoredTerminalWidget::getDarkColor(int colorindex) {
  switch (colorindex) {
    case 0 : return Qt::black;
    case 1 : return Qt::darkRed;
    case 2 : return Qt::darkGreen;
    case 3 : return Qt::darkYellow;
    case 4 : return Qt::darkBlue;
    case 5 : return Qt::darkMagenta;
    case 6 : return Qt::darkCyan;
    case 7 : return Qt::lightGray;
    default : Q_ASSERT(false); return QColor();
  }
}

QColor ColoredTerminalWidget::getRGBColor(QListIterator< QString > & i) {
  bool ok;
  if (!i.hasNext()) return QColor();
  int red = i.next().toInt(&ok);
  if (!ok || !i.hasNext()) return QColor();
  int green = i.next().toInt(&ok);
  if (!ok || !i.hasNext()) return QColor();
  int blue = i.next().toInt(&ok);
  return QColor(red, green, blue);
}

QColor ColoredTerminalWidget::getCubeColor(int index) {
  index -= 0x10;
  int red = index % 6;
  index /= 6;
  int green = index % 6;
  index /= 6;
  int blue = index % 6;
  index /= 6;
  Q_ASSERT(index == 0);
  return QColor(red, green, blue);
}

// based on https://stackoverflow.com/questions/26500429/qtextedit-and-colored-bash-like-output-emulation
// based on information: http://en.m.wikipedia.org/wiki/ANSI_escape_code http://misc.flogisoft.com/bash/tip_colors_and_formatting http://invisible-island.net/xterm/ctlseqs/ctlseqs.html
void ColoredTerminalWidget::parseEscapeSequence(int attribute, QListIterator< QString > & i, QTextCharFormat & textCharFormat, QTextCharFormat const & defaultTextCharFormat) {
  switch (attribute) {
    case 0 : { // Normal/Default (reset all attributes)
      textCharFormat = defaultTextCharFormat;
      break;
    }
    case 1 : { // Bold/Bright (bold or increased intensity)
      if (i.hasNext()) {
        while (i.hasNext()) {
          bool ok;
          int attributestack = i.next().toInt(&ok);
          parseEscapeSequence(attributestack, i, textCharFormat, defaultTextCharFormat);
        }
      } else {
        textCharFormat.setFontWeight(QFont::Bold);
      }
      break;
    }
    case 2 : { // Dim/Faint (decreased intensity)
      textCharFormat.setFontWeight(QFont::Light);
      break;
    }
    case 3 : { // Italicized (italic on)
      textCharFormat.setFontItalic(true);
      break;
    }
    case 4 : { // Underscore (single underlined)
      textCharFormat.setUnderlineStyle(QTextCharFormat::SingleUnderline);
      textCharFormat.setFontUnderline(true);
      break;
    }
    case 5 : { // Blink (slow, appears as Bold)
      textCharFormat.setFontWeight(QFont::Bold);
      break;
    }
    case 6 : { // Blink (rapid, appears as very Bold)
      textCharFormat.setFontWeight(QFont::Black);
      break;
    }
    case 7 : { // Reverse/Inverse (swap foreground and background)
      QBrush foregroundBrush = textCharFormat.foreground();
      textCharFormat.setForeground(textCharFormat.background());
      textCharFormat.setBackground(foregroundBrush);
      break;
    }
    case 8 : { // Concealed/Hidden/Invisible (usefull for passwords)
      textCharFormat.setForeground(textCharFormat.background());
      break;
    }
    case 9 : { // Crossed-out characters
      textCharFormat.setFontStrikeOut(true);
      break;
    }
    case 10 : { // Primary (default) font
      textCharFormat.setFont(defaultTextCharFormat.font());
      break;
    }
    case 11 ... 19 : {
      const QStringList families = textCharFormat.fontFamilies().toStringList();
      if (families.isEmpty())
        break;
      const QString& fontFamily = families.first();

      const QStringList fontStyles = QFontDatabase::styles(fontFamily);

      const int fontStyleIndex = attribute - 11;
      if (fontStyleIndex >= 0 && fontStyleIndex < fontStyles.size()) {
        const QFont cur = textCharFormat.font();
        const int pt = cur.pointSize() > 0 ? cur.pointSize() : qRound(cur.pointSizeF());

        textCharFormat.setFont(
            QFontDatabase::font(fontFamily,fontStyles.at(fontStyleIndex), pt)
        );
      }
      break;
    }
    case 20 : { // Fraktur (unsupported)
      break;
    }
    case 21 : { // Set Bold off
      textCharFormat.setFontWeight(QFont::Normal);
      break;
    }
    case 22 : { // Set Dim off
      textCharFormat.setFontWeight(QFont::Normal);
      break;
    }
    case 23 : { // Unset italic and unset fraktur
      textCharFormat.setFontItalic(false);
      break;
    }
    case 24 : { // Unset underlining
      textCharFormat.setUnderlineStyle(QTextCharFormat::NoUnderline);
      textCharFormat.setFontUnderline(false);
      break;
    }
    case 25 : { // Unset Blink/Bold
      textCharFormat.setFontWeight(QFont::Normal);
      break;
    }
    case 26 : { // Reserved
      break;
    }
    case 27 : { // Positive (non-inverted)
      QBrush backgroundBrush = textCharFormat.background();
      textCharFormat.setBackground(textCharFormat.foreground());
      textCharFormat.setForeground(backgroundBrush);
      break;
    }
    case 28 : {
      textCharFormat.setForeground(defaultTextCharFormat.foreground());
      textCharFormat.setBackground(defaultTextCharFormat.background());
      break;
    }
    case 29 : {
      textCharFormat.setUnderlineStyle(QTextCharFormat::NoUnderline);
      textCharFormat.setFontUnderline(false);
      break;
    }
    case 30 ... 37 : {
      int colorIndex = attribute - 30;
      QColor color;
      if (QFont::Normal < textCharFormat.fontWeight()) {
        color = getLightColor(colorIndex);
      } else {
        color = getDarkColor(colorIndex);
      }
      textCharFormat.setForeground(color);
      break;
    }
    case 38 : {
      if (i.hasNext()) {
        bool ok = false;
        int selector = i.next().toInt(&ok);
        Q_ASSERT(ok);
        QColor color;
        switch (selector) {
          case 2 : {
            textCharFormat.setForeground(getRGBColor(i));
            break;
          }
          case 5 : {
            if (!i.hasNext()) break;
            int index = i.next().toInt(&ok);
            Q_ASSERT(ok);
            switch (index) {
              case 0x00 ... 0x07 : { // 0x00-0x07:  standard colors (as in ESC [ 30..37 m)
                return parseEscapeSequence(index - 0x00 + 30, i, textCharFormat, defaultTextCharFormat);
              }
              case 0x08 ... 0x0F : { // 0x08-0x0F:  high intensity colors (as in ESC [ 90..97 m)
                return parseEscapeSequence(index - 0x08 + 90, i, textCharFormat, defaultTextCharFormat);
              }
              case 0x10 ... 0xE7 : { // 0x10-0xE7:  6*6*6=216 colors: 16 + 36*r + 6*g + b (0≤r,g,b≤5)
                color = getCubeColor(index);
                break;
              }
              case 0xE8 ... 0xFF : { // 0xE8-0xFF:  grayscale from black to white in 24 steps
                qreal intensity = qreal(index - 0xE8) / (0xFF - 0xE8);
                color.setRgbF(intensity, intensity, intensity);
                break;
              }
            }
            textCharFormat.setForeground(color);
            break;
          }
          default : {
            break;
          }
        }
      }
      break;
    }
    case 39 : {
      textCharFormat.setForeground(defaultTextCharFormat.foreground());
      break;
    }
    case 40 ... 47 : {
      int colorIndex = attribute - 40;
      QColor color = getLightColor(colorIndex);
      textCharFormat.setBackground(color);
      break;
    }
    case 48 : {
      if (i.hasNext()) {
        bool ok = false;
        int selector = i.next().toInt(&ok);
        Q_ASSERT(ok);
        QColor color;
        switch (selector) {
          case 2 : {
            textCharFormat.setBackground(getRGBColor(i));
            break;
          }
          case 5 : {
            if (!i.hasNext()) break;
            int index = i.next().toInt(&ok);
            Q_ASSERT(ok);
            switch (index) {
              case 0x00 ... 0x07 : { // 0x00-0x07:  standard colors (as in ESC [ 40..47 m)
                return parseEscapeSequence(index - 0x00 + 40, i, textCharFormat, defaultTextCharFormat);
              }
              case 0x08 ... 0x0F : { // 0x08-0x0F:  high intensity colors (as in ESC [ 100..107 m)
                return parseEscapeSequence(index - 0x08 + 100, i, textCharFormat, defaultTextCharFormat);
              }
              case 0x10 ... 0xE7 : { // 0x10-0xE7:  6*6*6=216 colors: 16 + 36*r + 6*g + b (0≤r,g,b≤5)
                color = getCubeColor(index);
                break;
              }
              case 0xE8 ... 0xFF : { // 0xE8-0xFF:  grayscale from black to white in 24 steps
                qreal intensity = qreal(index - 0xE8) / (0xFF - 0xE8);
                color.setRgbF(intensity, intensity, intensity);
                break;
              }
            }
            textCharFormat.setBackground(color);
            break;
          }
          default : {
            break;
          }
        }
      }
      break;
    }
    case 49 : {
      textCharFormat.setBackground(defaultTextCharFormat.background());
      break;
    }
    case 90 ... 97 : {
      int colorIndex = attribute - 90;
      QColor color = getLightColor(colorIndex);
      color.setRedF(color.redF() * 0.8);
      color.setGreenF(color.greenF() * 0.8);
      color.setBlueF(color.blueF() * 0.8);
      textCharFormat.setForeground(color);
      break;
    }
    case 100 ... 107 : {
      int colorIndex = attribute - 100;
      QColor color = getLightColor(colorIndex);
      color.setRedF(color.redF() * 0.8);
      color.setGreenF(color.greenF() * 0.8);
      color.setBlueF(color.blueF() * 0.8);
      textCharFormat.setBackground(color);
      break;
    }
    default : {
      break;
    }
  }
}

void ColoredTerminalWidget::setTextTermFormatting(QString const & text, const QTextCharFormat& defaultTextCharFormat) {
  // see https://gist.github.com/fnky/458719343aabd01cfb17a3a4f7296797
  // Name decimal octal hex  C-escape Ctrl-Key Description
  // BEL    7     007   0x07 \a       ^G       Terminal bell
  // BS     8     010   0x08 \b       ^H       Backspace
  // HT     9     011   0x09 \t       ^I       Horizontal TAB
  // LF    10     012   0x0A \n       ^J       Linefeed (newline)
  // VT    11     013   0x0B \v       ^K       Vertical TAB
  // FF    12     014   0x0C \f       ^L       Formfeed (also: New page NP)
  // CR    13     015   0x0D \r       ^M       Carriage return
  // ESC   27     033   0x1B \e*      ^[       Escape character
  // DEL  127     177   0x7F <none>   <none>   Delete character
  QRegularExpression const escapeSequenceExpression(R"((\x1B\[|[\x07\x08\x0B\x0C\x0D]))"); // not tab (\x09) and not newline (\x0A)

  QRegularExpression const escapeSequenceExpressionFormat(R"(([\d;]*)m)");
  QRegularExpression const escapeSequenceExpressionControl(R"(([\d;]*)([ABCDEFGHJKsru]))");

  QTextCursor cursor(document());

  cursor.movePosition(QTextCursor::End);
  QTextCharFormat textCharFormat = cursor.charFormat();
  int offset = -1;
  int previousOffset = 0;

  cursor.beginEditBlock();
  while (offset < text.size()) {
    QRegularExpressionMatch match;
    int escapepos = text.indexOf(escapeSequenceExpression, previousOffset, &match);
    if (escapepos >= 0) {
      offset = escapepos;
    } else {
      offset = text.size();
    }

    cursor.insertText(text.mid(previousOffset, (offset-previousOffset)), textCharFormat);
    previousOffset += (offset-previousOffset);


    if (match.hasMatch()) {
      // Move the cursor past the first match
      previousOffset = match.capturedEnd();

      // Compare against ESC+[ (CSI). Use QStringView-friendly literal.
      if (match.capturedView() == u"\x1B[") {
        QRegularExpressionMatch match_sequence;

        // Try to match a CSI "format" sequence starting exactly at previousOffset
        if ((match_sequence = escapeSequenceExpressionFormat.match(
               text,
               previousOffset,
               QRegularExpression::NormalMatch,
               QRegularExpression::AnchorAtOffsetMatchOption)).hasMatch()) {
          previousOffset = match_sequence.capturedEnd();

          // The last capture contains the semicolon-separated parameter list.
          const QString params = match_sequence.capturedTexts().back();
          const QStringList capturedTexts = params.split(QLatin1Char(';'), Qt::SkipEmptyParts);

          if (!capturedTexts.isEmpty()) {
            // Iterate parameters (e.g. SGR attributes like 0,1,31, ...)
            QListIterator<QString> it(capturedTexts);
            while (it.hasNext()) {
              bool ok = false;
              const int attribute = it.next().toInt(&ok);
              if (ok) {
                // Your handler that updates textCharFormat appropriately
                parseEscapeSequence(attribute, it, textCharFormat, defaultTextCharFormat);
              } else {
                // Convert QStringView to QString for safe concatenation
                qWarning().nospace()
                    << "Error in escape sequence \""
                    << match.capturedView()
                    << match_sequence.capturedView()
                    << "\"; falling back to default formatting.";
                textCharFormat = defaultTextCharFormat;
              }
            }
          } else {
            // Empty parameter list means reset (SGR 0)
            textCharFormat = defaultTextCharFormat;
          }
        }
        // Try other control sequences (non-formatting); also anchored at previousOffset
        else if ((match_sequence = escapeSequenceExpressionControl.match(
                    text,
                    previousOffset,
                    QRegularExpression::NormalMatch,
                    QRegularExpression::AnchorAtOffsetMatchOption)).hasMatch()) {
          previousOffset = match_sequence.capturedEnd();
          qDebug() << "Control detected" << match_sequence.capturedView();
          qDebug() << "ignoring for now";
        }
      } else {
        // General single-character controls (BEL, CR, etc.)
        qDebug() << "General detected" << match.capturedView();

        if (match.capturedView() == u"\a") {
          // BEL – beep / visual flash
          qWarning() << "beep";
          auto *timeLine = new QTimeLine(350, this);
          timeLine->setFrameRange(0, 255);

          // Brief viewport flash: animate background brightness
          connect(timeLine, &QTimeLine::frameChanged, [this](int frame) {
            QPalette p = this->viewport()->palette();
            p.setColor(this->viewport()->backgroundRole(), QColor(frame, frame, frame));
            this->viewport()->setPalette(p);
            // If needed on your widget: this->viewport()->setAutoFillBackground(true);
          });
          connect(timeLine, &QTimeLine::finished, timeLine, &QTimeLine::deleteLater);
          timeLine->start();
        } else if (match.capturedView() == u"\r") {
          // CR – move to line start; here we map to newline
          qDebug() << "carriage return";
          cursor.insertText("\n", textCharFormat);
        } else {
          qDebug() << "ignoring for now";
        }
      }
    }
  }
  cursor.setCharFormat(textCharFormat);
  cursor.endEditBlock();
  if (not this->atbottom) {
    button->show();
  } else {
    button->hide();
  }
}

void ColoredTerminalWidget::scrollToBottom(void) {
  if (this->atbottom) {
    verticalScrollBar()->setValue( verticalScrollBar()->maximum() );
  }
}

void ColoredTerminalWidget::scrolledTo(int val) {
  this->atbottom = (val == verticalScrollBar()->maximum());
  if (this->atbottom) {
    button->hide();
  }
}
