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
#include <QGroupBox>
#include <QSpinBox>
#include <QLabel>

#include <properties/data/ambassador.h>
#include <properties/data/properties/primitive/longproperty.h>

#include <plugins/infrastructure/qpropertywidgetfactory/src/qpropertywidgetfactory.h>
#include <plugins/infrastructure/qpropertywidgetfactory/src/widgets/qvtiwidget_long.h>

#include "boxsizewidget.h"


BoxSizeWidget::BoxSizeWidget(Property* property, QWidget* parent)
    : QPropertyWidget(property, new QGroupBox("Size", parent)) {
  Ambassador *ambassador = getInnerAmbassador();

  QVBoxLayout *widgetlayout = new QVBoxLayout(getWidget());

  xwidget = setupDimensionWidgets(ambassador, "X", "left", "right");
  ywidget = setupDimensionWidgets(ambassador, "Y", "bottom", "top");
  zwidget = setupDimensionWidgets(ambassador, "Z", "back", "front");
  widgetlayout->addWidget(xwidget);
  widgetlayout->addWidget(ywidget);
  widgetlayout->addWidget(zwidget);

  widgetlayout->addStretch(0);
}

QWidget* BoxSizeWidget::setupDimensionWidgets(Ambassador* ambassador, const char* dim, const std::string& lowername, const std::string& uppername) {
  QPropertyWidgetFactory *factory = QPropertyWidgetFactory::getInstance();

  QWidget *widget = new QWidget();

  QHBoxLayout *layout  = new QHBoxLayout(widget);

  layout->addWidget(new QLabel(dim));

  LongProperty *lowerproperty = ambassador->getProperty<LongProperty>(lowername);
  QVTIWidget_long *lowerspinbox = factory->createWidget<QVTIWidget_long>(lowerproperty->getValueTypeInterface());
  layout->addWidget(lowerspinbox->getWidget());

  layout->addWidget(new QLabel("to"));

  LongProperty *upperproperty = ambassador->getProperty<LongProperty>(uppername);
  QVTIWidget *upperspinbox = factory->createWidget<QVTIWidget>(upperproperty->getValueTypeInterface());
  layout->addWidget(upperspinbox->getWidget());

  connect(lowerspinbox, &QVTIWidget_long::valueChanged, this, [lowerproperty, upperproperty](QWidgetInterface* qwi) {
    long value = lowerproperty->getValue();
    upperproperty->updateHint()->setEntry("limit_min", value);
    static_cast<QVTIWidget*>(qwi)->synchronizeVTI(); // is lowerspinbox
  });

  connect(upperspinbox, &QVTIWidget_long::valueChanged, this, [lowerproperty, upperproperty](QWidgetInterface* qwi) {
    long value = upperproperty->getValue();
    lowerproperty->updateHint()->setEntry("limit_max", value);
    static_cast<QVTIWidget*>(qwi)->synchronizeVTI(); // is upperspinbox
  });

  return widget;
}

void BoxSizeWidget::synchronizeVTI() {
}

void BoxSizeWidget::setXRowEnabled(bool enabled) {
  xwidget->setEnabled(enabled);
}

void BoxSizeWidget::setYRowEnabled(bool enabled) {
  ywidget->setEnabled(enabled);
}

void BoxSizeWidget::setZRowEnabled(bool enabled) {
  zwidget->setEnabled(enabled);
}
