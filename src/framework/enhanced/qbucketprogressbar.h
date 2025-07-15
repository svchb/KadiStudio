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
#include <QStyleOption>
#include <QStyle>
#include <QAbstractAnimation>
#include <QDateTime>
#include <QDebug>
#include <QProgressBar>

#include <cpputils/dllapi.hpp>

#define MINIMUM 0

class QBucketProgressBarAnimation;

/**
 * @brief      Provides a progress bar widget with different buckets
 * @ingroup    qbucketprogrssbar
 */
class DLLAPI QBucketProgressBar : public QWidget {
  Q_OBJECT

  public:
    QBucketProgressBar();
    ~QBucketProgressBar() override = default;

    QWidget* getWidget();

    int value() const;
    int value(const uint& bucket) const;
    int minimum() const;
    int minimum(const uint& bucket) const;
    int maximum() const;
    int maximum(const uint& bucket) const;
    QColor color(const uint& bucket) const;
    bool active(const uint& bucket) const;

    QString text() const;
    void setTextVisible(bool visible);
    bool isTextVisible() const;
    Qt::Alignment alignment() const;
    void setAlignment(Qt::Alignment alignment);
    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;
    Qt::Orientation orientation() const;
    void setInvertedAppearance(bool invert);
    bool invertedAppearance() const;
    void setTextDirection(QProgressBar::Direction textDirection);
    QProgressBar::Direction textDirection() const;
    void setFormat(const QString& format);
    void resetFormat();
    QString format() const;

    void setStyle(QStyle* style) = delete;

  public Q_SLOTS:
    void setValue(const uint& bucket, const int& value, const bool& forceValue = false);
    void setRange(const uint& bucket, const int& min, const int& max);
    void setMinimum(const uint& bucket, const int& min);
    void setMaximum(const uint& bucket, const int& max);
    void setColor(const uint& bucket, const QColor& color);
    void setActive(const uint& bucket, const bool& active);
    void reset(const uint& bucket);
    void setOrientation(Qt::Orientation orientation);
    int addBucket(const int& min = 0, const int& max = 100);

  Q_SIGNALS:
    void valueChanged(uint bucket, int value);

  protected:
    bool event(QEvent* event) override;
    void paintEvent(QPaintEvent *) override;
    void initStyleOption(QStyleOptionProgressBar* option) const;
    bool repaintRequired(const uint& bucket);
    void drawControl(QStyle::ControlElement element,
                     const QStyleOption* option,
                     QPainter* painter,
                     const QWidget* widget = nullptr) const;
    void drawPrimitive(QStyle::PrimitiveElement element,
                       const QStyleOption* option,
                       QPainter* painter,
                       const QWidget* widget = nullptr) const;

  private:
    void initDefaultFormat();
    void updateValue();
    void updateRange();

    struct Bucket {
      int min;
      int max;
      int value;
      QColor color;
      bool visible;
      int lastPaintedValue;
      friend QDebug operator<<(QDebug debug, const Bucket& bucket) {
        debug.nospace() << QString("Bucket(min: %1, max: %2, value: %3, color %4, visible: %5, lastPaintedValue: %6)")
                               .arg(bucket.min).arg(bucket.max).arg(bucket.value).arg(bucket.color.value())
                               .arg(bucket.visible).arg(bucket.lastPaintedValue);
        debug.space();
        return debug;
      }
    };
    struct BucketCheck {
      int result;
      int bucket;
    };

    QVector<Bucket> m_buckets;
    QProgressBar::Direction m_textDirection;
    Qt::Alignment m_alignment;
    Qt::Orientation m_orientation;
    QString m_format;
    int m_minimum;
    int m_maximum;
    int m_value;
    int m_lastPaintedValue;
    QBucketProgressBar::BucketCheck m_lastRepaintCheck;
    bool m_invertedAppearance;
    bool m_textVisible;
    bool m_defaultFormat;

    friend class QBucketProgressBarStyle;
};

class QBucketProgressBarStyle : public QStyle {
  public:
    void drawComplexControl(QStyle::ComplexControl control,
                            const QStyleOptionComplex* option,
                            QPainter* painter,
                            const QWidget* widget = nullptr) const override;
    void drawControl(QStyle::ControlElement element,
                     const QStyleOption* option,
                     QPainter* painter,
                     const QWidget* widget = nullptr) const override;
    void drawItemPixmap(QPainter* painter,
                        const QRect& rectangle,
                        int alignment,
                        const QPixmap& pixmap) const override;
    void drawItemText(QPainter* painter,
                      const QRect& rectangle,
                      int alignment,
                      const QPalette& palette,
                      bool enabled,
                      const QString& text,
                      QPalette::ColorRole textRole = QPalette::NoRole) const override;
    void drawPrimitive(QStyle::PrimitiveElement element,
                       const QStyleOption* option,
                       QPainter* painter,
                       const QWidget* widget = nullptr) const override;
    QPixmap generatedIconPixmap(QIcon::Mode iconMode,
                                const QPixmap& pixmap,
                                const QStyleOption* option) const override;
    QStyle::SubControl hitTestComplexControl(QStyle::ComplexControl control,
                                             const QStyleOptionComplex* option,
                                             const QPoint& position,
                                             const QWidget* widget = nullptr) const override;
    QRect itemPixmapRect(const QRect& rectangle,
                         int alignment,
                         const QPixmap& pixmap) const override;
    QRect itemTextRect(const QFontMetrics& metrics,
                       const QRect& rectangle,
                       int alignment,
                       bool enabled,
                       const QString& text) const override;
    int layoutSpacing(QSizePolicy::ControlType control1,
                      QSizePolicy::ControlType control2,
                      Qt::Orientation orientation,
                      const QStyleOption* option = nullptr,
                      const QWidget* widget = nullptr) const override;
    int pixelMetric(QStyle::PixelMetric metric,
                    const QStyleOption* option = nullptr,
                    const QWidget* widget = nullptr) const override;
    void polish(QWidget* widget) override;
    void polish(QApplication* application) override;
    void polish(QPalette& palette) override;
    QSize sizeFromContents(QStyle::ContentsType type,
                           const QStyleOption* option,
                           const QSize& contentsSize,
                           const QWidget* widget = nullptr) const override;
    QPixmap standardPixmap(QStyle::StandardPixmap standardPixmap,
                           const QStyleOption* opt = nullptr,
                           const QWidget* widget = nullptr) const override;
    QIcon standardIcon(QStyle::StandardPixmap standardIcon,
                       const QStyleOption* option = 0,
                       const QWidget* widget = 0) const override;
    QPalette standardPalette() const override;
    int styleHint(QStyle::StyleHint hint,
                  const QStyleOption* option = nullptr,
                  const QWidget* widget = nullptr,
                  QStyleHintReturn* returnData = nullptr) const override;
    QRect subControlRect(QStyle::ComplexControl control,
                         const QStyleOptionComplex* option,
                         QStyle::SubControl subControl,
                         const QWidget* widget = nullptr) const override;
    QRect subElementRect(QStyle::SubElement element,
                         const QStyleOption* option,
                         const QWidget* widget = nullptr) const override;
    void unpolish(QWidget* widget) override;
    void unpolish(QApplication* application) override;
};


class QBucketProgressBarAnimation : public QAbstractAnimation {
  Q_OBJECT

  public:
    explicit QBucketProgressBarAnimation(QObject* parent = nullptr);
    int duration() const override;
    qreal speed();
    void setSpeed(qreal speed);
    void start(QAbstractAnimation::DeletionPolicy policy = QAbstractAnimation::KeepWhenStopped);
    void stop();
    void updateCurrentTime(int currentTime) override;

    enum FrameRate {
      DefaultFps = 60,
      SixtyFps = 60,
      ThirtyFps = 30,
      TwentyFps = 20,
      FifteenFps = 15
    };
    FrameRate frameRate();
    void setFrameRate(FrameRate fps);

  private:
    class QAnimatedBucketGradient : public QWidget {
      public:
        QAnimatedBucketGradient(QWidget* parent = nullptr);

        qreal speed();
        int delay();
        FrameRate fps();

        void setSpeed(const qreal& speed);
        void setDelay(const int& delay);
        void setFps(const FrameRate& fps);

      protected:
        void paintEvent(QPaintEvent *) override;
        void timerEvent(QTimerEvent *) override;

      private:
        int m_width;
        qreal m_animationValue;
        QDateTime m_startTime;
        qreal m_speed;
        int m_delay;
        FrameRate m_fps;
        bool m_vertical;
        bool m_reverse;
        bool m_inverted;
    };

  private:
    QAnimatedBucketGradient *m_gradient;
    int *m_timerId;
};
