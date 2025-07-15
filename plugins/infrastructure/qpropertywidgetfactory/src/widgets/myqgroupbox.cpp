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

#include <QFormLayout>
#include <QStyle>
#include <QLabel>
#include <QLayout>

#include "myqgroupbox.h"

GroupboxEventFilter::GroupboxEventFilter(QObject* parent) : QObject(parent) {
}

bool GroupboxEventFilter::eventFilter(QObject* obj, QEvent* event) { // TODO kann warscheinlich raus, da kein Unterschied zu eventfilter zu p3dwidget
  switch (event->type()) {

    case QEvent::FocusIn:
      emit gotFocus();
      break;

    case QEvent::FocusOut:
      emit lostFocus();
      break;

    default:;
  }
  return QObject::eventFilter(obj, event);
}

MyQGroupBox::MyQGroupBox(const QString& name, QLayout* layout, QWidget* parent) : QGroupBox(name, parent) {
  GroupboxEventFilter *filter = new GroupboxEventFilter(parent);

  // allow update description
  this->installEventFilter(filter);

  // QFormLayout *childlayout = new QFormLayout();
  //
  setLayout(layout);
  // layout()->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);
  // layout()->setLabelAlignment(Qt::AlignRight);
  setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);//QSizePolicy::Minimum);

  // In case of collapsed show dots
  QLabel *label = new QLabel("...");
  label->hide();
  this->layout()->addWidget(label);

  connect(filter, SIGNAL(gotFocus()),          this, SIGNAL(gotFocus()));
  connect(filter, SIGNAL(lostFocus()),         this, SIGNAL(lostFocus()));
  connect(this,   SIGNAL(gotFocus()),          this, SLOT(sendFocus()));
  connect(this,   SIGNAL(lostFocus()),         this, SLOT(looseFocus()));
  connect(this,   SIGNAL(toggled(bool)),       this, SLOT(checkToggled(bool)));

  this->setFocusPolicy(Qt::StrongFocus);
}

void MyQGroupBox::sendFocus() {
//   this->setStyleSheet("QGroupBox { border-style: inset; border-width: 2px; border-color: rgb(40, 120, 255); border-radius: 0.2em 0.2em }");
  this->setStyleSheet("QGroupBox { background-color: white; }");
}
void MyQGroupBox::looseFocus() {
  setStyleSheet("");
}

void MyQGroupBox::setCheckable(bool checkable) {
  QGroupBox::setCheckable(checkable);
}

void MyQGroupBox::checkToggled(bool check) {
  QList<QWidget*>           childrenList = this->findChildren<QWidget*>();
  QList<QWidget*>::iterator currChild    = childrenList.begin();

  if (check) {
    // for the first element in the children list. In our case the first element is always the 3 dots (...) of the groupbox
    (*currChild)->hide();
  } else {
    (*currChild)->show();
  }
  currChild++;

  for (; currChild != childrenList.end(); currChild++) {
    if ((*currChild)->parent() == this) {
      if (check) {
        (*currChild)->show();
      } else {
        (*currChild)->hide();
      }
    }
  }
}
