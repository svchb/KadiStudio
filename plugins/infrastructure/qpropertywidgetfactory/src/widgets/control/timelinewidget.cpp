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

#include <QLayout>
#include <QLabel>
#include <QPushButton>

#include <properties/data/ambassador.h>

#include <framework/pluginframework/pluginmanager.h>
#include <plugins/infrastructure/qpropertywidgetfactory/widgetfactoryinterface.h>
#include <plugins/infrastructure/qpropertywidgetfactory/src/widgets/qvtiwidget_bool.h>
#include <plugins/infrastructure/qpropertywidgetfactory/src/widgets/qvtiwidget_float.h>
#include <plugins/infrastructure/qpropertywidgetfactory/src/widgets/qvtiwidget_long.h>
#include <plugins/infrastructure/qpropertywidgetfactory/src/widgets/qvtiwidget_combobox.h>

#include "timelinewidget.h"
#include "properties/data/properties/primitive/longproperty.h"

TimelineWidget::TimelineWidget(Property* property, QWidget* parent)
    : QPropertyWidget(property, new QGroupBox("Timeline", parent)) {

  WidgetFactoryInterface *factory = LibFramework::PluginManager::getInstance()->getInterface<WidgetFactoryInterface*>("/plugins/infrastructure/qpropertywidgetfactory");

  QVBoxLayout *widgetlayout = new QVBoxLayout(getWidget());

  QHBoxLayout *timesteplayout = new QHBoxLayout();
  QLabel *timesteplabel = new QLabel(tr("Timestep: "));
  QVTIWidget_long *timestep = factory->createWidget<QVTIWidget_long>(getInnerAmbassador()->getValueTypeInterface("timestep"));
  timesteplayout->addWidget(timesteplabel);
  timesteplayout->addWidget(timestep->getWidget());

  timesteplayout->addWidget(new QLabel("of"));

  long maxtimestep;
  getInnerAmbassador()->getProperty("timestep")->getHint()->getEntry("limit_max", maxtimestep);
  maxtimesteplabel = new QLabel(QString::number(maxtimestep));
  timesteplayout->addWidget(maxtimesteplabel);
  widgetlayout->addLayout(timesteplayout);

  QHBoxLayout *buttonlayout   = new QHBoxLayout();
  QPushButton *backwardbutton = new QPushButton("⏮");
  backwardbutton->setToolTip(tr("Skip back"));
  buttonlayout->addWidget(backwardbutton);
  QPushButton *playpausebutton = new QPushButton("⏵⏸");
  playpausebutton->setToolTip(tr("Play / Pause"));
  buttonlayout->addWidget(playpausebutton);
  QPushButton *stopbutton      = new QPushButton("⏹");
  stopbutton->setToolTip(tr("Stop"));
  buttonlayout->addWidget(stopbutton);
  QPushButton *forwardbutton   = new QPushButton("⏭");
  forwardbutton->setToolTip(tr("Next"));
  buttonlayout->addWidget(forwardbutton);

  widgetlayout->addLayout(buttonlayout);

  QHBoxLayout *timelinefeaturelayout = new QHBoxLayout();
  QVTIWidget_bool *checkboxRecord = factory->createWidget<QVTIWidget_bool>(getInnerAmbassador()->getValueTypeInterface("record"));
  timelinefeaturelayout->addWidget(checkboxRecord->getWidget());

  QVTIWidget_bool *checkboxRepeat = factory->createWidget<QVTIWidget_bool>(getInnerAmbassador()->getValueTypeInterface("repeat"));
  timelinefeaturelayout->addWidget(checkboxRepeat->getWidget());

  QVTIWidget_float *spinnerFrameDelay = factory->createWidget<QVTIWidget_float>(getInnerAmbassador()->getValueTypeInterface("delay"));
  timelinefeaturelayout->addWidget(spinnerFrameDelay->getWidget());
  widgetlayout->addLayout(timelinefeaturelayout);

  // <delay> seconds between each animation
  timeline = new QTimeLine(getInnerAmbassador()->getValue<float>("delay"), this);

  QHBoxLayout *timelineslayout = new QHBoxLayout();
  QLabel *timelineslabel = new QLabel("Timelines: ");
  timelineslayout->addWidget(timelineslabel);
  QVTIWidget_combobox *timelinecombobox = factory->createWidget<QVTIWidget_combobox>(getInnerAmbassador()->getValueTypeInterface("timelines"));
  timelineslayout->addWidget(timelinecombobox->getWidget());
  widgetlayout->addLayout(timelineslayout);

  connect(timeline, SIGNAL(frameChanged(int)), this, SLOT(timestepChanged(int)));
  connect(backwardbutton, SIGNAL(clicked()), this, SLOT(rewindFrames()));
  connect(playpausebutton, SIGNAL(pressed()), this, SLOT(playTimeline()));
  connect(stopbutton, SIGNAL(pressed()), this, SLOT(stopTimeline()));
  connect(forwardbutton, SIGNAL(clicked()), this, SLOT(fastForwardFrames()));
}

void TimelineWidget::synchronizeVTI() {
  long maxtimestep;
  getInnerAmbassador()->getProperty("timestep")->getHint()->getEntry("limit_max", maxtimestep);
  maxtimesteplabel->setText(QString::number(maxtimestep));
}

void TimelineWidget::timestepChanged(int number) {
  long maxtimestep;
  getInnerAmbassador()->getProperty("timestep")->getHint()->getEntry("limit_max", maxtimestep);

  if (number == maxtimestep) {
    int minimum = 1;//frameslider->minimum();
    timeline->setStartFrame(minimum);
  } else {
    timeline->setStartFrame(number);
  }
}

void TimelineWidget::playTimeline() {
  QTimeLine::State state = timeline->state();

  if ((state == QTimeLine::Paused) ||
      (state == QTimeLine::NotRunning)) {
    timeline->start();
  } else if (state == QTimeLine::Running) {
    timeline->setPaused(true);
  }
}

void TimelineWidget::stopTimeline() {
  timeline->stop();
  int minimum = 1;//frameslider->minimum();
  LongProperty *timestepproperty = getInnerAmbassador()->getProperty<LongProperty>("timestep");
  timestepproperty->setValue(minimum);
}

void TimelineWidget::rewindFrames() {
  loadTenPercent(false);
}

void TimelineWidget::fastForwardFrames() {
  loadTenPercent(true);
}

void TimelineWidget::loadTenPercent(bool next) {
  LongProperty *timestepproperty = getInnerAmbassador()->getProperty<LongProperty>("timestep");
  long maxtimestep;
  timestepproperty->getHint()->getEntry("limit_max", maxtimestep);
  int minimum      = 1;//frameslider->minimum();
  int currentframe = timestepproperty->getValue();

  if ((next && (currentframe == maxtimestep)) ||
      (!next && (currentframe == minimum))) {
    return;
  }

  long ten_percent = std::max(1l, maxtimestep / 10);

  long newframe = currentframe;

  if (next) {
    newframe += ten_percent;
  } else {
    newframe -= ten_percent;
  }

  if (newframe > maxtimestep) {
    timestepproperty->setValue(maxtimestep);
  } else if (newframe < minimum) {
    timestepproperty->setValue(minimum);
  } else {
    timestepproperty->setValue(newframe);
  }
}
