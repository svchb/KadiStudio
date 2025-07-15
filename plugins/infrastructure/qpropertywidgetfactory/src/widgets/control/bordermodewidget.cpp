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

#include <string>
#include <vector>

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QComboBox>

#include <properties/data/ambassador.h>

#include "bordermodewidget.h"


BorderModeWidget::BorderModeWidget(Property* property, QWidget* parent)
    : QPropertyWidget(property, new QGroupBox("Border Modes", parent)) {

  QVBoxLayout *widgetlayout = new QVBoxLayout(getWidget());

  QStringList bordermodes;
  std::vector<std::string> modes;
  getInnerAmbassador()->getValue("modes", modes);
  for (const std::string& mode : modes) {
    bordermodes << QString::fromStdString(mode);
  }

  QHBoxLayout *leftlayout = new QHBoxLayout();
  QLabel *leftlabel = new QLabel(tr("Left (lower X)"));
  leftcombobox = new QComboBox();
  leftcombobox->addItems(bordermodes);
  leftlayout->addWidget(leftlabel);
  leftlayout->addWidget(leftcombobox);
  widgetlayout->addLayout(leftlayout);

  QHBoxLayout *bottomlayout = new QHBoxLayout();
  QLabel *bottomlabel = new QLabel(tr("Bottom (lower Y)"));
  bottomcombobox = new QComboBox();
  bottomcombobox->addItems(bordermodes);
  bottomlayout->addWidget(bottomlabel);
  bottomlayout->addWidget(bottomcombobox);
  widgetlayout->addLayout(bottomlayout);

  QHBoxLayout *backlayout = new QHBoxLayout();
  QLabel *backlabel = new QLabel(tr("Back (lower Z)"));
  backcombobox = new QComboBox();
  backcombobox->addItems(bordermodes);
  backlayout->addWidget(backlabel);
  backlayout->addWidget(backcombobox);
  widgetlayout->addLayout(backlayout);

  QHBoxLayout *rightlayout = new QHBoxLayout();
  QLabel *rightlabel = new QLabel(tr("Right (higher X)"));
  rightcombobox = new QComboBox();
  rightcombobox->addItems(bordermodes);
  rightlayout->addWidget(rightlabel);
  rightlayout->addWidget(rightcombobox);
  widgetlayout->addLayout(rightlayout);

  QHBoxLayout *toplayout = new QHBoxLayout();
  QLabel *toplabel = new QLabel(tr("Top (higher Y)"));
  topcombobox = new QComboBox();
  topcombobox->addItems(bordermodes);
  toplayout->addWidget(toplabel);
  toplayout->addWidget(topcombobox);
  widgetlayout->addLayout(toplayout);

  QHBoxLayout *frontlayout = new QHBoxLayout();
  QLabel *frontlabel = new QLabel(tr("Front (higher Z)"));
  frontcombobox = new QComboBox();
  frontcombobox->addItems(bordermodes);
  frontlayout->addWidget(frontlabel);
  frontlayout->addWidget(frontcombobox);
  widgetlayout->addLayout(frontlayout);
  widgetlayout->addStretch(0);

  connect(leftcombobox, SIGNAL(currentIndexChanged(int)), this, SLOT(setBorderModeLeft(int)));
  connect(bottomcombobox, SIGNAL(currentIndexChanged(int)), this, SLOT(setBorderModeBottom(int)));
  connect(backcombobox, SIGNAL(currentIndexChanged(int)), this, SLOT(setBorderModeBack(int)));
  connect(rightcombobox, SIGNAL(currentIndexChanged(int)), this, SLOT(setBorderModeRight(int)));
  connect(topcombobox, SIGNAL(currentIndexChanged(int)), this, SLOT(setBorderModeTop(int)));
  connect(frontcombobox, SIGNAL(currentIndexChanged(int)), this, SLOT(setBorderModeFront(int)));
}

BorderModeWidget::~BorderModeWidget() {
}

void BorderModeWidget::synchronizeVTI() {
  std::array<long, 6> bordermodes;
  getInnerAmbassador()->getValue("bordermodes", bordermodes);
  leftcombobox->setCurrentIndex(bordermodes[0]);
  bottomcombobox->setCurrentIndex(bordermodes[1]);
  backcombobox->setCurrentIndex(bordermodes[2]);
  rightcombobox->setCurrentIndex(bordermodes[3]);
  topcombobox->setCurrentIndex(bordermodes[4]);
  frontcombobox->setCurrentIndex(bordermodes[5]);
}

void BorderModeWidget::setBorderMode(int mode, int side) {
  std::array<long, 6> bordermodes;
  getInnerAmbassador()->getValue("bordermodes", bordermodes);
  bordermodes[side] = mode;
  getInnerAmbassador()->setValue("bordermodes", bordermodes);
}

void BorderModeWidget::setBorderModeLeft(int mode) {
  setBorderMode(mode, 0);
}

void BorderModeWidget::setBorderModeBottom(int mode) {
  setBorderMode(mode, 1);
}

void BorderModeWidget::setBorderModeBack(int mode) {
  setBorderMode(mode, 2);
}

void BorderModeWidget::setBorderModeRight(int mode) {
  setBorderMode(mode, 3);
}

void BorderModeWidget::setBorderModeTop(int mode) {
  setBorderMode(mode, 4);
}

void BorderModeWidget::setBorderModeFront(int mode) {
  setBorderMode(mode, 5);
}
