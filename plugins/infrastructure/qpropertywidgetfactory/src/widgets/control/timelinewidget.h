/* Copyright 2022 Karlsruhe Institute of Technology
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

#include <QGroupBox>
#include <QTimeLine>
#include "../../../qpropertywidget.h"
class QLabel;


/**
 *  @brief      A widget for controlling the timelines in a
 *              maskfile.
 *  @ingroup    qtpropertywidgetfactory
 */
class TimelineWidget : public QPropertyWidget {
    Q_OBJECT

  public:
    TimelineWidget(Property* property, QWidget* parent = NULL);
    virtual ~TimelineWidget() = default;

    void synchronizeVTI() override;

  private Q_SLOTS:
    void timestepChanged(int value);
    void playTimeline();
    void stopTimeline();
    void rewindFrames();
    void fastForwardFrames();

  private:
    void loadTenPercent(bool next);

    QLabel *maxtimesteplabel;
    QTimeLine *timeline;

};
