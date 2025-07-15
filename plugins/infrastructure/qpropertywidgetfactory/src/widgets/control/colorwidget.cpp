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
#include <QMap>

#include <properties/data/ambassador.h>
#include <properties/data/properties/controls/colormapproperty.h>

#include <plugins/infrastructure/qpropertywidgetfactory/src/qpropertywidgetfactory.h>
#include <plugins/infrastructure/qpropertywidgetfactory/qvtiwidget.h>

#include "colorlabel.h"
#include "colorgradientlabel.h"
#include "colorwidget.h"


ColorWidget::ColorWidget(Property* property, QWidget* parent)
    : QPropertyWidget(property, new QGroupBox("Color", parent)) {

  QPropertyWidgetFactory *factory = QPropertyWidgetFactory::getInstance();

  QVBoxLayout *widgetlayout = new QVBoxLayout(getWidget());

  colorgradientlabel = new ColorGradientLabel;

  auto ambassador = getInnerAmbassador();

  colorlabel        = new ColorLabel;
  colorlabel->setSize(40, 20);

  scalarupperlabel       = new QLabel(" 0,75");
  scalarmiddlelabel      = new QLabel(" 0,5");
  scalarlowerlabel       = new QLabel(" 0,25");

  QVBoxLayout *scalarlayout = new QVBoxLayout();
  QVTIWidget *scalarmaxfloatspinbox = factory->createWidget<QVTIWidget>(ambassador->getValueTypeInterface("colormax"));
  scalarlayout->addWidget(scalarmaxfloatspinbox->getWidget());
  scalarlayout->addWidget(scalarupperlabel);
  scalarlayout->addWidget(scalarmiddlelabel);
  scalarlayout->addWidget(scalarlowerlabel);
  QVTIWidget *scalarminfloatspinbox = factory->createWidget<QVTIWidget>(ambassador->getValueTypeInterface("colormin"));
//   scalarminfloatspinbox->setSingleStep(0.01);
  scalarlayout->addWidget(scalarminfloatspinbox->getWidget());

  QHBoxLayout *colorgradientlayout = new QHBoxLayout();
  colorgradientlayout->addWidget(colorgradientlabel);
  colorgradientlayout->addLayout(scalarlayout);
  widgetlayout->addLayout(colorgradientlayout);

  QHBoxLayout *colormodelayout = new QHBoxLayout();
  QVTIWidget *colormodecombobox = factory->createWidget<QVTIWidget>(ambassador->getValueTypeInterface("colormap/colormode"));
  colormodelayout->addWidget(colormodecombobox->getWidget());
  colormodelayout->addWidget(colorlabel);
  widgetlayout->addLayout(colormodelayout);

  QVTIWidget *invertcheckbox = factory->createWidget<QVTIWidget>(ambassador->getValueTypeInterface("colormap/invert"));
  widgetlayout->addWidget(invertcheckbox->getWidget());
  QVTIWidget *negatecheckbox = factory->createWidget<QVTIWidget>(ambassador->getValueTypeInterface("colormap/negate"));
  widgetlayout->addWidget(negatecheckbox->getWidget());

  widgetlayout->addStretch(0);

  ambassador->registerPropertyChangeListener(this);

  Property *colormapproperty = ambassador->getProperty("colormap");
  Ambassador *colormap = dynamic_cast<Ambassador*>(colormapproperty);
  colormap->registerPropertyChangeListener(this);

  connect(colorlabel, &ColorLabel::colorChanged, this, [this](QColor color) {
    ColorMapProperty *colormapproperty = getInnerAmbassador()->getProperty<ColorMapProperty>("colormap");
    colormapproperty->setConstColor(color.red(), color.green(), color.blue());
    calculateColorSteps();
  });
}

ColorWidget::~ColorWidget() {
}

void ColorWidget::synchronizeVTI() {
  ColorMapProperty *colormapproperty = getInnerAmbassador()->getProperty<ColorMapProperty>("colormap");

  toggleColorLabel(colormapproperty->getColorMode());

  unsigned char red, green, blue;
  colormapproperty->getConstColor(red, green, blue);
  QColor customcolor(red, green, blue);
  colorlabel->setColor(customcolor);

  calculateColorSteps();
}

void ColorWidget::receivePropertyChange(const Property* property) {
  auto ambassador = getInnerAmbassador();

  if (property->hasName("colormode")) {
    std::string mode;
    ambassador->getValue("colormap/colormode", mode);
    toggleColorLabel(mode);
  } else if (property->hasName("constcolor")) {
    ColorMapProperty *colormapproperty = getInnerAmbassador()->getProperty<ColorMapProperty>("colormap");
    unsigned char red, green, blue;
    colormapproperty->getConstColor(red, green, blue);
    QColor customcolor(red, green, blue);
    colorlabel->setColor(customcolor);
  }

  calculateColorSteps();
}

void ColorWidget::calculateColorSteps() {
  auto ambassador = getInnerAmbassador();

  float min, max;
  ambassador->getValue("colormin", min);
  ambassador->getValue("colormax", max);

  double mid  = (max + min) * 0.5;
  double cmin = (mid + min) * 0.5;
  double cmax = (max + mid) * 0.5;

  scalarupperlabel->setNum(cmax);
  scalarmiddlelabel->setNum(mid);
  scalarlowerlabel->setNum(cmin);

  ColorMapProperty *colormapproperty = getInnerAmbassador()->getProperty<ColorMapProperty>("colormap");

  unsigned char color[4];

  QMap<double, QColor> colorsteps;
  colormapproperty->getColorNormalized(color, 0.0);  // min
  colorsteps.insert(0.0,  QColor(color[0], color[1], color[2]));
  colormapproperty->getColorNormalized(color, 0.25); // cmin
  colorsteps.insert(0.25, QColor(color[0], color[1], color[2]));
  colormapproperty->getColorNormalized(color, 0.5);  // mid
  colorsteps.insert(0.5,  QColor(color[0], color[1], color[2]));
  colormapproperty->getColorNormalized(color, 0.75); // cmax
  colorsteps.insert(0.75, QColor(color[0], color[1], color[2]));
  colormapproperty->getColorNormalized(color, 1.0);  // max
  colorsteps.insert(1.0,  QColor(color[0], color[1], color[2]));
  colorgradientlabel->setColorSteps(colorsteps);
}

void ColorWidget::toggleColorLabel(const std::string& mode) {
  if (mode == "const") {
    qDebug("ColorWidget::toggleColorLabel");
    colorlabel->setEnabled(true);
  } else {
    colorlabel->setDisabled(true);
  }
}
