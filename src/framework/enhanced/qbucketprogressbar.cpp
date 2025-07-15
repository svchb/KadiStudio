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

#include <QAccessibleValueChangeEvent>
#include <QApplication>
#include <QStylePainter>
#include <QStyle>
#include <QPainter>
#include <QtWidgets>

#include "qbucketprogressbar.h"

QBucketProgressBar::QBucketProgressBar()
    : m_textDirection(QProgressBar::TopToBottom), m_alignment(Qt::AlignCenter),
      m_orientation(Qt::Horizontal), m_minimum(MINIMUM), m_maximum(m_minimum), m_value(m_minimum - 1),
      m_lastPaintedValue(m_value), m_lastRepaintCheck({ 0, 0 }), m_invertedAppearance(false), m_textVisible(true),
      m_defaultFormat(true) {
  // set format
  initDefaultFormat();

  // set size policy
  QSizePolicy sp(QSizePolicy::Expanding, QSizePolicy::Fixed);
  if (m_orientation == Qt::Vertical) {
    sp.transpose();
  }
  setSizePolicy(sp);
  setAttribute(Qt::WA_WState_OwnSizePolicy, false);

  // set style
  QWidget::setStyle(new QBucketProgressBarStyle());
  setAttribute(Qt::WA_SetStyle, QWidget::style());

  // set palette
  auto pal = palette();
  pal.setColor(QPalette::Highlight, Qt::red);
  pal.setColor(QPalette::Dark, Qt::lightGray);
  pal.setColor(QPalette::Light, Qt::lightGray);
  pal.setColor(QPalette::Window, QColor(Qt::lightGray).lighter(120));
  pal.setColor(QPalette::Text, Qt::black);
  pal.setColor(QPalette::HighlightedText, Qt::white);
  setPalette(pal);

  // adds the first bucket
  addBucket();
}

QWidget* QBucketProgressBar::getWidget() {
  return this;
}

int QBucketProgressBar::value() const {
  return m_value;
}

int QBucketProgressBar::value(const uint& bucket) const {
  Q_ASSERT((int) bucket < m_buckets.size());
  return m_buckets[bucket].value;
}

int QBucketProgressBar::minimum() const {
  return m_minimum;
}

int QBucketProgressBar::minimum(const uint& bucket) const {
  Q_ASSERT((int) bucket < m_buckets.size());
  return m_buckets[bucket].min;
}


int QBucketProgressBar::maximum() const {
  return m_maximum;
}

int QBucketProgressBar::maximum(const uint& bucket) const {
  Q_ASSERT((int) bucket < m_buckets.size());
  return m_buckets[bucket].max;
}


QColor QBucketProgressBar::color(const uint& bucket) const {
  Q_ASSERT((int) bucket < m_buckets.size());
  return m_buckets[bucket].color;
}

bool QBucketProgressBar::active(const uint &bucket) const {
  Q_ASSERT((int) bucket < m_buckets.size());
  return m_buckets[bucket].visible;
}

QString QBucketProgressBar::text() const {
  if (m_maximum == m_minimum || m_value < m_minimum
      || (m_value == INT_MIN && m_minimum == INT_MIN)) {
    return QString();
  }
  qint64 totalSteps = qint64(m_maximum) - m_minimum;
  QString result = m_format;
  QLocale loc = locale();
  loc.setNumberOptions(loc.numberOptions() | QLocale::OmitGroupSeparator);
  result.replace(QLatin1String("%m"), loc.toString(totalSteps));
  result.replace(QLatin1String("%v"), loc.toString(m_value));
  if (totalSteps == 0) {
    result.replace(QLatin1String("%p"), loc.toString(100));
    return result;
  }
  const auto progress = static_cast<int>((qint64(m_value) - m_minimum) * 100.0 / totalSteps);
  result.replace(QLatin1String("%p"), loc.toString(progress));
  return result;
}

void QBucketProgressBar::setTextVisible(bool visible) {
  if (m_textVisible != visible) {
    m_textVisible = visible;
    repaint();
  }
}

bool QBucketProgressBar::isTextVisible() const {
  return m_textVisible;
}

Qt::Alignment QBucketProgressBar::alignment() const {
  return m_alignment;
}

void QBucketProgressBar::setAlignment(Qt::Alignment alignment) {
  if (m_alignment != alignment) {
    m_alignment = alignment;
    repaint();
  }
}

QSize QBucketProgressBar::sizeHint() const {
  ensurePolished();
  QFontMetrics fm = fontMetrics();
  QStyleOptionProgressBar opt;
  initStyleOption(&opt);
  int cw = style()->pixelMetric(QStyle::PM_ProgressBarChunkWidth, &opt, this);
  QSize size = QSize(qMax(9, cw) * 7 + fm.horizontalAdvance(QLatin1Char('0')) * 4, fm.height() + 8);
  if (opt.orientation == Qt::Vertical) {
    size = size.transposed();
  }
  return style()->sizeFromContents(QStyle::CT_ProgressBar, &opt, size, this);
}

QSize QBucketProgressBar::minimumSizeHint() const {
  QSize size;
  if (orientation() == Qt::Horizontal) {
    size = QSize(sizeHint().width(), fontMetrics().height() + 2);
  } else {
    size = QSize(fontMetrics().height() + 2, sizeHint().height());
  }
  return size;
}

Qt::Orientation QBucketProgressBar::orientation() const {
  return m_orientation;
}

void QBucketProgressBar::setInvertedAppearance(bool invert) {
  if (m_invertedAppearance != invert) {
    m_invertedAppearance = invert;
    update();
  }
}

bool QBucketProgressBar::invertedAppearance() const {
  return m_invertedAppearance;
}

void QBucketProgressBar::setTextDirection(QProgressBar::Direction textDirection) {
  if (m_textDirection != textDirection) {
    m_textDirection = textDirection;
    update();
  }
}

QProgressBar::Direction QBucketProgressBar::textDirection() const {
  return m_textDirection;
}

void QBucketProgressBar::setFormat(const QString& format) {
  if (m_format != format) {
    m_defaultFormat = false;
    m_format = format;
    update();
  }
}

void QBucketProgressBar::resetFormat() {
  m_defaultFormat = true;
  initDefaultFormat();
  update();
}

QString QBucketProgressBar::format() const {
  return m_format;
}

/// slots
void QBucketProgressBar::setValue(const uint& bucket, const int& value, const bool& forceValue) {
  Q_ASSERT((int) bucket < m_buckets.size());

  Bucket *b = &m_buckets[bucket];
  if (b->value == value) return;
  if (forceValue) {
    if (value > b->max) setMaximum(bucket, value);
    if (value < b->min) setMinimum(bucket, value);
  } else if (value > b->max || value < b->min) {
    return;
  }

  b->value = value;
  updateValue();
  emit valueChanged(bucket, value);
#ifndef QT_NO_ACCESSIBILITY
  if (isVisible()) {
    QAccessibleValueChangeEvent event(this, value);
    QAccessible::updateAccessibility(&event);
  }
#endif
  if (repaintRequired(bucket)) {
    repaint();
  }
}

void QBucketProgressBar::setRange(const uint& bucket, const int& min, const int& max) {
  Q_ASSERT((int) bucket < m_buckets.size());

  Bucket *b = &m_buckets[bucket];
  if (min != b->min || max != b->max) {
    b->min = min;
    b->max = qMax(min, max);
    updateRange();
    if (b->value < qint64(b->min) - 1 || b->value > b->max) {
      reset(bucket);
    } else {
      update();
    }
  }
}

void QBucketProgressBar::setMinimum(const uint& bucket, const int& min) {
  Q_ASSERT((int) bucket < m_buckets.size());
  setRange(bucket, min, m_buckets[bucket].max);
}

void QBucketProgressBar::setMaximum(const uint& bucket, const int& max) {
  Q_ASSERT((int) bucket < m_buckets.size());
  setRange(bucket, m_buckets[bucket].min, max);
}

void QBucketProgressBar::setColor(const uint& bucket, const QColor& color) {
  Q_ASSERT((int) bucket < m_buckets.size());

  Bucket *b = &m_buckets[bucket];
  if (b->color != color) {
    b->color = color;
    repaint();
  }
}

void QBucketProgressBar::setActive(const uint& bucket, const bool& active) {
  Q_ASSERT((int) bucket < m_buckets.size());

  Bucket *b = &m_buckets[bucket];
  if (b->visible != active) {
    b->visible = active;
    updateRange();
    updateValue();
    repaint();
  }
}

void QBucketProgressBar::setOrientation(Qt::Orientation orientation) {
  if (m_orientation == orientation) return;

  m_orientation = orientation;
  if (orientation == Qt::Vertical) {
    setAlignment(Qt::AlignCenter);
  }
  if (!testAttribute(Qt::WA_WState_OwnSizePolicy)) {
    setSizePolicy(sizePolicy().transposed());
    setAttribute(Qt::WA_WState_OwnSizePolicy, false);
  }
  update();
  updateGeometry();
}

void QBucketProgressBar::reset(const uint &bucket) {
  Q_ASSERT((int) bucket < m_buckets.size());

  const Bucket b = m_buckets[bucket];
  if (b.min == INT_MIN) {
    setValue(bucket, INT_MIN);
  } else {
    setValue(bucket, b.min);
  }
}

int QBucketProgressBar::addBucket(const int& min, const int& max) {
  m_buckets.append(Bucket());

  int bucket = m_buckets.size() - 1;
  setRange(bucket, min, max);
  reset(bucket);
  setColor(bucket, QColor(Qt::green).darker(100 + (bucket % 5) * 40));
  setActive(bucket, true);
  m_buckets[bucket].lastPaintedValue = m_buckets[bucket].value;

  return bucket;
}


bool QBucketProgressBar::event(QEvent *event) {
  switch (event->type()) {
    case QEvent::LocaleChange:
      initDefaultFormat();
      break;
    default:
      break;
  }
  return QWidget::event(event);
}


void QBucketProgressBar::paintEvent(QPaintEvent*) {
  int pos = m_lastRepaintCheck.bucket;
  int res = m_lastRepaintCheck.result;
  if (res - 2) m_buckets[pos].lastPaintedValue = m_buckets[pos].value;
  if (res % 2 > 0) m_lastPaintedValue = m_value;

  QStylePainter paint(this);
  QStyleOptionProgressBar opt;
  initStyleOption(&opt);
  paint.drawControl(QStyle::CE_ProgressBar, opt);
}

void QBucketProgressBar::initStyleOption(QStyleOptionProgressBar* option) const {
  if (!option) {
    return;
  }

  option->initFrom(this);
  if (m_orientation == Qt::Horizontal) {
    option->state |= QStyle::State_Horizontal;
  }
  option->minimum = m_minimum;
  option->maximum = m_maximum;
  option->progress = m_value;
  option->textAlignment = m_alignment;
  option->textVisible = m_textVisible;
  option->text = text();
  option->orientation = m_orientation;  // ### Qt 6: remove this member from QStyleOptionProgressBar
  option->invertedAppearance = m_invertedAppearance;
  option->bottomToTop = m_textDirection == QProgressBar::BottomToTop;
}

bool QBucketProgressBar::repaintRequired(const uint& bucket) {
  Q_ASSERT((int) bucket < m_buckets.size());

  int retVal = 0;
  // Check if the text needs to be repainted
  {
    const auto valueDifference = qAbs(qint64(m_value) - m_lastPaintedValue);
    if (m_value == m_minimum || m_value == m_maximum) {
      retVal += 1;
    } else {
      const auto totalSteps = qint64(m_maximum) - m_minimum;
      if (m_textVisible) {
        if ((m_format.contains(QLatin1String("%v")))) {
          retVal += 1;
        } else if ((m_format.contains(QLatin1String("%p")) && valueDifference >= qAbs(totalSteps / 100))) {
          retVal += 1;
        }
      }
    }
  }
  // Check if the bar needs to be repainted
  {
    QStyleOptionProgressBar opt;
    initStyleOption(&opt);

    Bucket *b = const_cast<Bucket*>(&m_buckets[bucket]);
    const auto valueDifference = qAbs(qint64(b->value) - b->lastPaintedValue);
    if (b->value == b->min || b->value == b->max) {
      retVal += 2;
    } else {
      const qint64 totalSteps = qint64(b->max) - b->min;
      qreal cw = style()->pixelMetric(QStyle::PM_ProgressBarChunkWidth, &opt, this);
      QRect groove = style()->subElementRect(QStyle::SE_ProgressBarGroove, &opt, this);
      int grooveBlock = (orientation() == Qt::Horizontal) ? groove.width() : groove.height();
      int distance = m_maximum - m_minimum;
      qreal groovePart = (grooveBlock / (distance > 0 ? distance : 1)) * (totalSteps > 0 ? totalSteps : 1);
      retVal += (valueDifference * groovePart > cw * totalSteps) ? 2 : 0;
    }

  }
  m_lastRepaintCheck = { retVal, (int) bucket };
  return retVal;
}

void QBucketProgressBar::initDefaultFormat() {
  if (m_defaultFormat) {
    m_format = QLatin1String("%p") + locale().percent();
  }
}

void QBucketProgressBar::updateValue() {
  m_value = MINIMUM;
  for (const Bucket &b : m_buckets) {
    if (b.visible && b.value >= b.min && b.value <= b.max) {
      m_value += abs(b.value - b.min);
    }
  }
}

void QBucketProgressBar::updateRange() {
  m_minimum = MINIMUM;
  m_maximum = MINIMUM;
  for (const Bucket &b : m_buckets) {
    if (b.visible) {
      m_maximum += b.max - b.min;
    }
  }
}

void QBucketProgressBarStyle::drawComplexControl(QStyle::ComplexControl control,
                                                 const QStyleOptionComplex* option,
                                                 QPainter* painter,
                                                 const QWidget* widget) const {
  QApplication::style()->drawComplexControl(control, option, painter, widget);
}


void QBucketProgressBarStyle::drawControl(QStyle::ControlElement element,
                                          const QStyleOption* option,
                                          QPainter* painter,
                                          const QWidget* widget) const {
  switch (element) {
    case QStyle::CE_ProgressBarLabel:
      if (const QStyleOptionProgressBar *pb = qstyleoption_cast<const QStyleOptionProgressBar *>(option)) {
        const bool vertical = pb->orientation == Qt::Vertical;
        QPalette::ColorRole textRole = QPalette::Text;
        if ((pb->textAlignment & Qt::AlignCenter || vertical) && pb->textVisible
            && ((qint64(pb->progress) - qint64(pb->minimum)) * 2 >= (qint64(pb->maximum) - qint64(pb->minimum)))) {
          textRole = QPalette::HighlightedText;
          //Draw text shadow, This will increase readability when the background of same color
          QRect shadowRect(pb->rect);
          shadowRect.translate(1, 1);
          QColor shadowColor = (pb->palette.color(textRole).value() <= 128)
                                   ? QColor(255, 255, 255, 160) : QColor(0, 0, 0, 16);
          QPalette shadowPalette = pb->palette;
          shadowPalette.setColor(textRole, shadowColor);
          proxy()->drawItemText(painter, shadowRect, Qt::AlignCenter | Qt::TextSingleLine, shadowPalette,
                                pb->state & State_Enabled, pb->text, textRole);
        }
        proxy()->drawItemText(painter, pb->rect, Qt::AlignCenter | Qt::TextSingleLine, pb->palette,
                              pb->state & State_Enabled, pb->text, textRole);
      }
      break;

    case QStyle::CE_ProgressBarGroove:
      if (option->rect.isValid())
          qDrawShadePanel(painter, option->rect, option->palette, true, 1,
                          &option->palette.brush(QPalette::Window));
      break;

    case QStyle::CE_ProgressBarContents:
      if (const QStyleOptionProgressBar *pb = qstyleoption_cast<const QStyleOptionProgressBar*>(option)) {
          if (pb->minimum != pb->maximum) {
            QRect rect = pb->rect;
            QRectF r = subElementRect(QStyle::SE_ProgressBarLayoutItem, pb, widget);
            if (!r.isValid()) {
              r = rect;
              rect = QRect(r.x() + 1, r.y() + (pb->orientation == Qt::Vertical ? 0 : 1),
                           r.width() - 2, r.height() - 2);
            }

            const bool vertical = pb->orientation == Qt::Vertical;
            const bool inverted = pb->invertedAppearance;
            qint64 minimum = qint64(pb->minimum);
            qint64 maximum = qint64(pb->maximum);
            qint64 progress = qint64(pb->progress) - minimum;
            QPalette pal2 = pb->palette;

            // Correct the highlight color if it is the same as the background
            if (pal2.highlight() == pal2.window()) {
              pal2.setColor(QPalette::Highlight, pb->palette.color(QPalette::Active,
                                                                   QPalette::Highlight));
            }

            bool reverse = ((!vertical && (pb->direction == Qt::RightToLeft)) || vertical);

            QStyleOptionProgressBar pbBits = *pb;
            pbBits.rect = rect;
            pbBits.palette = pal2;
            pbBits.state = State_None;
            int myHeight;
            int myWidth;
            int myX = rect.x();
            int myY = rect.y();
            if (vertical) {
              myHeight = (rect.height() / qreal(maximum - minimum == 0 ? 1.0 : maximum - minimum)) * progress;
              myWidth = rect.width();
              myY = 1 + ((inverted) ? rect.bottom() + 2 - myHeight : rect.top());
            } else {
              myHeight = rect.height();
              myWidth = (rect.width() / qreal(maximum - minimum == 0 ? 1.0 : maximum - minimum)) * progress;
              if (reverse != inverted) {
                myX = rect.right() - myWidth + 1;
              }
            }
            pbBits.rect.setRect(myX, myY, myWidth, myHeight);

#if QT_CONFIG(animation)
            QBucketProgressBarAnimation *animation = nullptr;
            for (QObject *obj : option->styleObject->children()) {
              if ((animation = qobject_cast<QBucketProgressBarAnimation*>(obj))) {
                break;
              }
            }

            if (animation) {
              if (progress == maximum) {
                animation->stop();
              } else if (animation->state() == QAbstractAnimation::Stopped) {
                animation->start();
              }
            } else {
              QBucketProgressBarAnimation *animation = new QBucketProgressBarAnimation(option->styleObject);
              animation->start();
            }
#endif

            QBucketProgressBar *bar = qobject_cast<QBucketProgressBar*>(option->styleObject);
            // draw all visible buckets
            if (bar && progress > 0) {
              qint64 buckPos = 0;
              int dif = 0;
              for (QBucketProgressBar::Bucket &b : bar->m_buckets) {
                if (b.visible) {
                  dif += ((b.value - b.min) / (qreal) progress) *
                         ((vertical) ? pbBits.rect.height() : pbBits.rect.width());
                }
              }
              dif = ((vertical) ? pbBits.rect.height() : pbBits.rect.width()) - dif;

              for (int i = 0; i < bar->m_buckets.size(); i++) {
                QBucketProgressBar::Bucket b = bar->m_buckets[i];
                int bucketProgress = b.value - b.min;
                if (b.visible) {
                  QStyleOptionProgressBar bucket = pbBits;
                  QPalette palB = bucket.palette;
                  palB.setColor(QPalette::Highlight, b.color);
                  bucket.palette = palB;

                  QRect r = bucket.rect;
                  if (!vertical) {
                    int w = (bucketProgress / (qreal) progress) * r.width();
                    if (dif > 0 && bucketProgress > 0) {
                      w++;
                      dif--;
                    }
                    bucket.rect = QRect(r.x() + buckPos, r.y(), w, r.height());
                    buckPos += w;
                  } else {
                    int h = (bucketProgress / (qreal) progress) * r.height();
                    if (dif > 0 && bucketProgress > 0) {
                      h++;
                      dif--;
                    }
                    bucket.rect = QRect(r.x(), r.y() + buckPos, r.width(), h);
                    buckPos += h;
                  }
                  proxy()->drawPrimitive(PE_IndicatorProgressChunk, &bucket, painter, widget);
                }
              }
            }
            // daraw a single rectangle for the entire progress indicator
            else {
              proxy()->drawPrimitive(PE_IndicatorProgressChunk, &pbBits, painter, widget);
            }
        }
      }
      break;

    case QStyle::CE_ProgressBar:
      if (const QStyleOptionProgressBar *pb = qstyleoption_cast<const QStyleOptionProgressBar *>(option)) {
        QStyleOptionProgressBar subopt = *pb;
        subopt.rect = subElementRect(SE_ProgressBarGroove, pb, widget);
        proxy()->drawControl(CE_ProgressBarGroove, &subopt, painter, widget);
        subopt.rect = subElementRect(SE_ProgressBarContents, pb, widget);
        proxy()->drawControl(CE_ProgressBarContents, &subopt, painter, widget);
        if (pb->textVisible) {
          subopt.rect = subElementRect(SE_ProgressBarLabel, pb, widget);
          proxy()->drawControl(CE_ProgressBarLabel, &subopt, painter, widget);
        }
      }
      break;

    default:
      QApplication::style()->drawControl(element, option, painter, widget);
      break;
  }
}

void QBucketProgressBarStyle::drawItemPixmap(QPainter* painter,
                                             const QRect& rectangle,
                                             int alignment,
                                             const QPixmap& pixmap) const {
  QApplication::style()->drawItemPixmap(painter, rectangle, alignment, pixmap);
}

void QBucketProgressBarStyle::drawItemText(QPainter *painter,
                                           const QRect &rectangle,
                                           int alignment,
                                           const QPalette &palette,
                                           bool enabled,
                                           const QString &text,
                                           QPalette::ColorRole textRole) const {
  QApplication::style()->drawItemText(painter, rectangle, alignment, palette, enabled, text, textRole);
}

void QBucketProgressBarStyle::drawPrimitive(QStyle::PrimitiveElement element,
                                            const QStyleOption *option,
                                            QPainter *painter,
                                            const QWidget *widget) const {
  switch (element) {
    case QStyle::PE_IndicatorProgressChunk:
    {
      bool vertical = false, inverted = false;
      if (const QStyleOptionProgressBar *pb = qstyleoption_cast<const QStyleOptionProgressBar *>(option)) {
        vertical = pb->orientation == Qt::Vertical;
        inverted = pb->invertedAppearance;
      }

      if (!vertical) {
        if (inverted) {
          painter->fillRect(option->rect.x(), option->rect.y(), option->rect.width(),
                            option->rect.height(), option->palette.brush(QPalette::Highlight));
        } else {
          painter->fillRect(option->rect.x(), option->rect.y(), option->rect.width(),
                            option->rect.height(), option->palette.brush(QPalette::Highlight));
        }
      } else {
        if (inverted) {
          painter->fillRect(option->rect.x(), option->rect.y(), option->rect.width(),
                            option->rect.height(), option->palette.brush(QPalette::Highlight));
        } else {
          painter->fillRect(option->rect.x(), option->rect.y(), option->rect.width(),
                            option->rect.height(), option->palette.brush(QPalette::Highlight));
        }
      }
      break;
    }
    default:
      QApplication::style()->drawPrimitive(element, option, painter, widget);
      break;
  }
}

QPixmap QBucketProgressBarStyle::generatedIconPixmap(QIcon::Mode iconMode,
                                                     const QPixmap &pixmap,
                                                     const QStyleOption *option) const {
  return QApplication::style()->generatedIconPixmap(iconMode, pixmap, option);
}

QStyle::SubControl QBucketProgressBarStyle::hitTestComplexControl(QStyle::ComplexControl control,
                                                                  const QStyleOptionComplex* option,
                                                                  const QPoint& position,
                                                                  const QWidget* widget) const {
  return QApplication::style()->hitTestComplexControl(control, option, position, widget);
}

QRect QBucketProgressBarStyle::itemPixmapRect(const QRect& rectangle,
                                              int alignment,
                                              const QPixmap& pixmap) const {
  return QApplication::style()->itemPixmapRect(rectangle, alignment, pixmap);
}

QRect QBucketProgressBarStyle::itemTextRect(const QFontMetrics &metrics,
                                            const QRect &rectangle,
                                            int alignment, bool enabled,
                                            const QString& text) const {
  return QApplication::style()->itemTextRect(metrics, rectangle, alignment, enabled, text);
}

int QBucketProgressBarStyle::layoutSpacing(QSizePolicy::ControlType control1,
                                           QSizePolicy::ControlType control2,
                                           Qt::Orientation orientation,
                                           const QStyleOption* option,
                                           const QWidget* widget) const {
  return QApplication::style()->layoutSpacing(control1, control2, orientation, option, widget);
}

int QBucketProgressBarStyle::pixelMetric(QStyle::PixelMetric metric,
                                         const QStyleOption* option,
                                         const QWidget* widget) const {
  return QApplication::style()->pixelMetric(metric, option, widget);
}

void QBucketProgressBarStyle::polish(QWidget* widget) {
  QApplication::style()->polish(widget);
}

void QBucketProgressBarStyle::polish(QApplication* application) {
  QApplication::style()->polish(application);
}

void QBucketProgressBarStyle::polish(QPalette& palette) {
  QApplication::style()->polish(palette);
}

QSize QBucketProgressBarStyle::sizeFromContents(QStyle::ContentsType type,
                                                const QStyleOption* option,
                                                const QSize& contentsSize,
                                                const QWidget* widget) const {
  return QApplication::style()->sizeFromContents(type, option, contentsSize, widget);
}

QPixmap QBucketProgressBarStyle::standardPixmap(QStyle::StandardPixmap standardPixmap,
                                                const QStyleOption* option,
                                                const QWidget* widget) const {
  return QApplication::style()->standardPixmap(standardPixmap, option, widget);
}

QIcon QBucketProgressBarStyle::standardIcon(QStyle::StandardPixmap standardIcon,
                                            const QStyleOption* option,
                                            const QWidget* widget) const {
  return QApplication::style()->standardIcon(standardIcon, option, widget);
}

QPalette QBucketProgressBarStyle::standardPalette() const {
  return QApplication::style()->standardPalette();
}


int QBucketProgressBarStyle::styleHint(QStyle::StyleHint hint,
                                       const QStyleOption* option,
                                       const QWidget* widget,
                                       QStyleHintReturn* returnData) const {
  return QApplication::style()->styleHint(hint, option, widget, returnData);
}

QRect QBucketProgressBarStyle::subControlRect(QStyle::ComplexControl control,
                                              const QStyleOptionComplex* option,
                                              QStyle::SubControl subControl,
                                              const QWidget* widget) const {
  return QApplication::style()->subControlRect(control, option, subControl, widget);
}

QRect QBucketProgressBarStyle::subElementRect(QStyle::SubElement element,
                                              const QStyleOption* option,
                                              const QWidget* widget) const {
  return QApplication::style()->subElementRect(element, option, widget);
}

void QBucketProgressBarStyle::unpolish(QWidget* widget) {
  QApplication::style()->unpolish(widget);
}

void QBucketProgressBarStyle::unpolish(QApplication* application) {
  QApplication::style()->unpolish(application);
}

QBucketProgressBarAnimation::QBucketProgressBarAnimation(QObject* parent)
    : QAbstractAnimation(parent), m_timerId(nullptr) {
  m_gradient = new QAnimatedBucketGradient(qobject_cast<QWidget*>(parent));
}

int QBucketProgressBarAnimation::duration() const {
  return -1;
}

qreal QBucketProgressBarAnimation::speed() {
  return m_gradient->speed();
}

void QBucketProgressBarAnimation::setSpeed(qreal speed) {
  m_gradient->setSpeed(speed);
}

void QBucketProgressBarAnimation::start(QAbstractAnimation::DeletionPolicy policy) {
  QAbstractAnimation::start(policy);
  if (m_timerId) {
    stop();
  }
  m_timerId = new int(m_gradient->startTimer(1000 / m_gradient->fps()));
  m_gradient->setVisible(true);
}

void QBucketProgressBarAnimation::stop() {
  QAbstractAnimation::stop();
  if (m_timerId) {
    m_gradient->killTimer(*m_timerId);
    delete m_timerId;
    m_timerId = nullptr;
  }
  m_gradient->setVisible(false);
}

void QBucketProgressBarAnimation::updateCurrentTime(int currentTime) {
  Q_UNUSED(currentTime);
}

QBucketProgressBarAnimation::FrameRate QBucketProgressBarAnimation::frameRate() {
  return m_gradient->fps();
}

void QBucketProgressBarAnimation::setFrameRate(FrameRate fps) {
  m_gradient->setFps(fps);
  if (state() == QAbstractAnimation::Running) {
    start();
  }
}

///--------------------------------------------------------------------------------------------------------------------
///
/// class QAnimatedGradient
///
///--------------------------------------------------------------------------------------------------------------------

QBucketProgressBarAnimation::QAnimatedBucketGradient::QAnimatedBucketGradient(QWidget* parent)
    : QWidget(parent), m_width(80), m_animationValue(-m_width), m_speed(1), m_delay(0), m_fps(DefaultFps),
  m_vertical(false), m_reverse(false), m_inverted(false) {
}

qreal QBucketProgressBarAnimation::QAnimatedBucketGradient::speed() {
  return m_speed;
}

int QBucketProgressBarAnimation::QAnimatedBucketGradient::delay() {
  return m_delay;
}

QBucketProgressBarAnimation::FrameRate QBucketProgressBarAnimation::QAnimatedBucketGradient::fps() {
  return m_fps;
}

void QBucketProgressBarAnimation::QAnimatedBucketGradient::setSpeed(const qreal& speed) {
  m_speed = speed;
}

void QBucketProgressBarAnimation::QAnimatedBucketGradient::setDelay(const int& delay) {
  m_delay = delay;
}

void QBucketProgressBarAnimation::QAnimatedBucketGradient::setFps(const FrameRate& fps) {
  m_fps = fps;
}

void QBucketProgressBarAnimation::QAnimatedBucketGradient::paintEvent(QPaintEvent*) {
  QPainter painter(this);
  painter.setPen(QPen(Qt::NoPen));
  painter.setRenderHint(QPainter::Antialiasing);

  QPointF start = QPointF(m_animationValue , 0);
  QPointF end = QPointF(m_animationValue + (width() < m_width ? m_width : width()), 0);
  QRectF r = QRectF(m_animationValue, 0, m_width, height());

  if (m_vertical) {
    start = start.transposed();
    end = end.transposed();
    if (m_inverted) {
      start = QPointF(0, -m_animationValue);
      end = QPointF (0, (width() < m_width ? m_width : width()) - m_animationValue);
      r = QRectF(0, 0, width(), height());
    } else {
      r = QRectF(0, m_animationValue, width(), m_width);
    }
  } else if (!m_vertical && m_reverse != m_inverted) {
    end = QPointF(-m_animationValue, 0);
    start = QPointF((width() < m_width ? m_width : width()) - m_animationValue, 0);
    r = QRectF(0, 0, width(), height());
  }

  QLinearGradient gradient(start, end);
  gradient.setColorAt(0, QColor(255, 255, 255, 0));
  gradient.setColorAt(((width() < m_width ? 1 : m_width / (qreal) width())) * 1/3., QColor(255, 255, 255, 120));
  gradient.setColorAt(((width() < m_width ? 1 : m_width / (qreal) width())) / 2., QColor(255, 255, 255, 180));
  gradient.setColorAt(((width() < m_width ? 1 : m_width / (qreal) width())) * 2/3., QColor(255, 255, 255, 120));
  gradient.setColorAt(((width() < m_width ? 1 : m_width /(qreal) width())), QColor(255, 255, 255, 0));

  painter.setBrush(gradient);
  painter.drawRect(r);
}


void QBucketProgressBarAnimation::QAnimatedBucketGradient::timerEvent(QTimerEvent*) {
  QBucketProgressBar *bar = qobject_cast<QBucketProgressBar*>(parent());
  QStyleOptionProgressBar option;
  option.initFrom(bar);

  if (bar->orientation() == Qt::Horizontal) {
    option.state |= QStyle::State_Horizontal;
  }
  option.minimum = bar->minimum();
  option.maximum = bar->maximum();
  option.progress = bar->value();
  option.textAlignment = bar->alignment();
  option.textVisible = bar->isTextVisible();
  option.text = bar->text();
  option.orientation = bar->orientation();  // ### Qt 6: remove this member from QStyleOptionProgressBar
  option.invertedAppearance = bar->invertedAppearance();
  option.bottomToTop = bar->textDirection() == QProgressBar::BottomToTop;

  m_vertical = bar->orientation() == Qt::Vertical;
  m_inverted = bar->invertedAppearance();
  m_reverse = ((!m_vertical && (option.direction == Qt::RightToLeft)) || m_vertical);

  QRectF dim = bar->style()->subElementRect(QStyle::SE_ProgressBarContents, &option, bar);
  QRectF r = bar->style()->subElementRect(QStyle::SE_ProgressBarLayoutItem, &option, bar);
  if (!r.isValid()) {
    dim = QRectF(dim.x() + 1, dim.y() + 1, dim.width() - 2, dim.height() - 2);
  }

  int prog = bar->value() - bar->minimum();
  int range = (bar->minimum() == bar->maximum()) ? 1 : bar->maximum() - bar->minimum();
  int myWidth;
  int myHeight;
  int boundary;
  if (m_vertical) {
    myWidth = dim.width();
    myHeight = (dim.height() / (qreal) range) * prog;
    boundary = dim.height();
    if (m_inverted) {
      setGeometry(dim.x(), dim.bottom() + 1 - myHeight, myWidth, myHeight);
    } else {
      setGeometry(dim.x(), dim.y(), myWidth, myHeight);
    }
  } else {
    myWidth = (dim.width() / (qreal) range) * prog;
    myHeight = dim.height();
    boundary = dim.width();
    if (m_inverted) {
      setGeometry(dim.right() + 1 - myWidth, dim.y(), myWidth, myHeight);
    } else {
      setGeometry(dim.x(), dim.y(), myWidth, myHeight);
    }
  }

  QDateTime currentTimeStamp = QDateTime::currentDateTime();
  int elapsed = m_startTime.msecsTo(currentTimeStamp);
  m_startTime = currentTimeStamp;

  int value = (elapsed / (1000.0 / (qreal) m_fps)) * 6.0 * m_speed * (qreal) DefaultFps / (qreal) m_fps;

  m_animationValue += value;
  if (m_animationValue > ((boundary < m_width) ? m_width : boundary)) {
    m_animationValue = - m_width;
  }

  update();
}
