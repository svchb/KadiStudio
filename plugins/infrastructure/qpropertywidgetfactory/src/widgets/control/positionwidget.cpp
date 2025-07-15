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

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QLabel>

#include <properties/data/ambassador.h>

#include <plugins/infrastructure/qpropertywidgetfactory/src/qpropertywidgetfactory.h>
#include <plugins/infrastructure/qpropertywidgetfactory/src/widgets/qvtiwidget_long.h>

#include "positionwidget.h"
#include "boxsizewidget.h"

PositionWidget::PositionWidget(Property* property, QWidget* parent)
    : QPropertyWidget(property, new QGroupBox("Position", parent)) {

  QPropertyWidgetFactory *factory = QPropertyWidgetFactory::getInstance();

  QHBoxLayout *widgetlayout = new QHBoxLayout(getWidget());
  QVBoxLayout *groupboxlayout = new QVBoxLayout();

  boxsizewidget = factory->createWidget<BoxSizeWidget>(getInnerAmbassador()->getProperty("boundingbox"));
  widgetlayout->addWidget(boxsizewidget->getWidget());

  xradionbutton = new QRadioButton("X");
  yradionbutton = new QRadioButton("Y");
  zradionbutton = new QRadioButton("Z");
  groupboxlayout->addWidget(xradionbutton);
  groupboxlayout->addWidget(yradionbutton);
  groupboxlayout->addWidget(zradionbutton);
  widgetlayout->addLayout(groupboxlayout);

  QHBoxLayout *positionlayout  = new QHBoxLayout();
  QLabel      *positionlabel   = new QLabel("position");
  QVTIWidget_long *positionspinbox = factory->createWidget<QVTIWidget_long>(getInnerAmbassador()->getValueTypeInterface("cut.position"));
  positionlayout->addWidget(positionlabel);
  positionlayout->addWidget(positionspinbox->getWidget());
  groupboxlayout->addLayout(positionlayout);

  connect(xradionbutton, SIGNAL(pressed()), this, SLOT(disableXSizeControls()));
  connect(yradionbutton, SIGNAL(pressed()), this, SLOT(disableYSizeControls()));
  connect(zradionbutton, SIGNAL(pressed()), this, SLOT(disableZSizeControls()));
}

PositionWidget::~PositionWidget() {
}

void PositionWidget::synchronizeVTI() {
  long  cutdirection;
  getInnerAmbassador()->getValue("cut.direction", cutdirection);

  if (cutdirection == 0) {
    disableXSizeControls();
    xradionbutton->setChecked(true);
  } else if (cutdirection == 1) {
    disableYSizeControls();
    yradionbutton->setChecked(true);
  } else if (cutdirection == 2) {
    disableZSizeControls();
    zradionbutton->setChecked(true);
  }
}

void PositionWidget::disableXSizeControls() {
  boxsizewidget->setXRowEnabled(false);
  boxsizewidget->setYRowEnabled(true);
  boxsizewidget->setZRowEnabled(true);
}

void PositionWidget::disableYSizeControls() {
  boxsizewidget->setXRowEnabled(true);
  boxsizewidget->setYRowEnabled(false);
  boxsizewidget->setZRowEnabled(true);
}

void PositionWidget::disableZSizeControls() {
  boxsizewidget->setXRowEnabled(true);
  boxsizewidget->setYRowEnabled(true);
  boxsizewidget->setZRowEnabled(false);
}
