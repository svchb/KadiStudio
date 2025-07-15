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

#include <cmath>

#include <QDebug>
#include <QKeyEvent>
#include <QProgressDialog>
#include <QProgressBar>
#include <QMessageBox>

#include <QPushButton>
#include <QSpinBox>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QScrollBar>

#include <plugins/infrastructure/qpropertywidgetfactory/qvtiwidget.h>

#include "editdialog_vector.h"
#include "plugins/infrastructure/qpropertywidgetfactory/qwidgetinterface.h"
#include "properties/data/propertyfactory.h"
#include "properties/data/valuetypeinterface/abstractvaluetypeinterface.h"

InfileWidget_VectorEditEventFilter::InfileWidget_VectorEditEventFilter(QObject* parent) : QObject(parent) {};

bool InfileWidget_VectorEditEventFilter::eventFilter(QObject* obj, QEvent* event) {
  // send signal to update descripton
  if (event->type() == QEvent::KeyPress) {
    QKeyEvent *keyevent = static_cast<QKeyEvent*>(event);
    if (keyevent->key() == Qt::Key_Return) {
      Q_EMIT triggeredEnter();
    }
    if (keyevent->key() == Qt::Key_Enter) {
      Q_EMIT triggeredEnter();
    }
    if (keyevent->key() == Qt::Key_Enter) {
        Q_EMIT triggeredEnter();
    }
    if (keyevent->modifiers () == Qt::ControlModifier) {
      if (keyevent->key() == Qt::Key_Return) {
        Q_EMIT apply();
      }
      if (keyevent->key() == Qt::Key_R) {
        Q_EMIT edit();
      }
      if (keyevent->key() == Qt::Key_I) {
        Q_EMIT shortjump();
      }
      if (keyevent->key() == Qt::Key_O) {
        Q_EMIT override();
      }
    }
    if (keyevent->modifiers () == Qt::AltModifier) {
      if (keyevent->key() == Qt::Key_Left) {
        Q_EMIT altLeft();
      }
      if (keyevent->key() == Qt::Key_Right) {
        Q_EMIT altRight();
      }
    }
  }

  // standard event processing
  return QObject::eventFilter(obj, event);
}

EditDialog_Vector::EditDialog_Vector() : EditDialog() {
  this->filter = new InfileWidget_VectorEditEventFilter(this);
  this->installEventFilter(this->filter);
  icon = new QIcon(":icons/barretr_Pencil.png");

  normal                = QFont();
  bold                  = QFont();
  bold.setBold(true);

  pal                   = QPalette();
  pal.setColor(QPalette::Base,   QColor(214, 255, 214));
  pal.setColor(QPalette::Window, QColor(214, 255, 214));

  lightPal              = QPalette();
  lightPal.setColor(QPalette::Base,   QColor(224, 235, 224));
  lightPal.setColor(QPalette::Window, QColor(224, 235, 224));

  setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

  focusposition         = 0;
  position              = 0;

  visiblelineeditstart  = 10;
  visiblelineedit       = visiblelineeditstart;

  mostleftedit          = 0;

  editable = true;

  // Flag for validate state of convertbox
  convertable = true;

  // creating Layouts
  toplevellayout  = new QVBoxLayout(this);
  setLayout(toplevellayout);

  editlayout      = new QGridLayout();
  scrolllayout    = new QHBoxLayout();
  jumpboxlayout   = new QHBoxLayout();
  controlslayout  = new QHBoxLayout();

  toplevellayout->addLayout(editlayout);
  toplevellayout->addLayout(scrolllayout);
  toplevellayout->addLayout(jumpboxlayout);
  toplevellayout->addStretch(1);
  toplevellayout->addLayout(controlslayout);

  // GUI initialation and setting of Tooltips
  hscrollbar      = new QScrollBar(this);
  hscrollbar->setOrientation(Qt::Horizontal);
  hscrollbar->setMinimum(0);
  hscrollbar->setToolTip(tr("Move Vector"));
  scrolllayout->addWidget(hscrollbar);

  jumpbutton      = new QPushButton(tr("Jump to"), this);
  jumpbutton->setToolTip(tr("Jump to index in vector"));
  jumpbutton->setAutoDefault(false);

  jumpmark        = new QSpinBox(this);
  jumpmark->setToolTip(tr("Shortcut: Ctrl + i"));
  jumpmark->setMinimum(0);

  lengthbutton    = new QPushButton(tr("Change Vector Size to"), this);
  lengthbutton->setToolTip(tr("Set vector length to value in box"));
  lengthbutton->setAutoDefault(false);
  lengthbutton->setEnabled(false);

  vectorlength    = new QSpinBox(this);
  vectorlength->setToolTip(tr("Shortcut: Ctrl + e"));
  vectorlength->setMinimum(1);
  vectorlength->setMaximum(INT_MAX);

  convertbutton   = new QPushButton(tr("Set All"), this);
  convertbutton->setToolTip(tr("Set vector values to value given in the box."));
  convertbutton->setAutoDefault(false);

  spacer = new QSpacerItem(1, 0, QSizePolicy::Expanding, QSizePolicy::Minimum);

  fillControl();

  connect(this->filter,        SIGNAL(apply()),              this, SLOT(apply()));
  connect(this->filter,        SIGNAL(triggeredEnter()),     this, SLOT(enterPressed()));
  connect(this->filter,        SIGNAL(shortjump()),          jumpmark, SLOT(setFocus()));
  connect(this->filter,        SIGNAL(edit()),               vectorlength, SLOT(setFocus()));
  connect(this->filter,        SIGNAL(altRight()),           this, SLOT(stepRight()));
  connect(this->filter,        SIGNAL(altLeft()),            this, SLOT(stepLeft()));
  connect(this->jumpbutton,    SIGNAL(clicked()),            this, SLOT(jumpTriggered()));
  connect(this->hscrollbar,    SIGNAL(valueChanged(int)),    this, SLOT(move()));
  connect(this->convertbutton, SIGNAL(clicked()),            this, SLOT(convert()));
  connect(this->lengthbutton,  SIGNAL(clicked()),            this, SLOT(resize()));

  setAccessibleName(tr("Edit"));
}

EditDialog_Vector::~EditDialog_Vector() {
}

void EditDialog_Vector::fillTitel() {
  QString  title;
  if (avti->getHint()->hasEntry("label")) {
    title += QString::fromStdString(avti->getHint()->getEntry("label")) + ": ";
  }

  AbstractVectorValueType *avvti = dynamic_cast<AbstractVectorValueType*>(avti);
  title += "vector of type ";
  title += demangle(avvti->getElementValueTypeInfo().name()).c_str();
  this->setWindowTitle(title);
}

void EditDialog_Vector::fillJump() {
  jumpboxlayout->addWidget(jumpbutton);
  jumpboxlayout->addWidget(jumpmark);

  jumpboxlayout->addWidget(lengthbutton);
  jumpboxlayout->addWidget(vectorlength);

  jumpboxlayout->addWidget(convertbutton);
  jumpboxlayout->addWidget(convertbox->getOuterWidget());
  jumpboxlayout->addItem(spacer);
}

void EditDialog_Vector::fillControl() {
  QPushButton *helpbutton      = new QPushButton(QIcon(":icons/help-browser.png"), "", this);
  helpbutton->setToolTip(tr("Shortcut summary"));
  helpbutton->setAutoDefault(false);
  controlslayout->addWidget(helpbutton);

  QSpacerItem *space = new QSpacerItem(0, 0, QSizePolicy::Expanding);
  controlslayout->addSpacerItem(space);

  QPushButton *okbutton        = new QPushButton(tr("Ok"), this);
  okbutton->setToolTip(tr("Apply your changes and leave editor"));
  okbutton->setAutoDefault(false);
  controlslayout->addWidget(okbutton);

  QPushButton *resetbutton     = new QPushButton(tr("Reset"), this);
  resetbutton->setToolTip(tr("Reset your changes"));
  resetbutton->setAutoDefault(false);
  controlslayout->addWidget(resetbutton);

  QPushButton *cancelbutton    = new QPushButton(tr("Cancel"), this);
  // cancelbutton->setFocus();
  cancelbutton->setToolTip(tr("Reset your changes and leave editor"));
  cancelbutton->setAutoDefault(false);
  controlslayout->addWidget(cancelbutton);

  connect(helpbutton,    SIGNAL(clicked()),            this, SLOT(help()));
  connect(okbutton,      SIGNAL(clicked()),            this, SLOT(apply()));
  connect(resetbutton,   SIGNAL(clicked()),            this, SLOT(reset()));
  connect(cancelbutton,  SIGNAL(clicked()),            this, SLOT(cancel()));
}

/** @brief returns number of ',' in @param parentstring
  */
int EditDialog_Vector::setStartSize(const QString& parentstring) {
  if (parentstring.size() <= 2) { // only parentess or less
    return 0;
  } else {
    return (parentstring.count(",")+1);
  }
}

/** filling of Vector with all values of parentstring
  */
void EditDialog_Vector::vectorise(const QString& parentstring) {
  // remove opening '(' and closing ')'
  QString vectorstring = parentstring;
  vectorstring.remove(0, 1);
  vectorstring.chop(1);
  vector.clear();
  vector = vectorstring.split(',');
}

void EditDialog_Vector::clearEdit() {
  for (int i=0; i<linelist.count(); i++) {
    editlayout->removeWidget(linelist[i]->getWidget());
    delete linelist[i];
    editlayout->removeWidget(labellist[i]);
    delete labellist[i];
  }

  linelist.clear();
  labellist.clear();

  // editlayout->removeItem(jumpboxlayout);
}

/** @brief create Label and Lineedits
  */
void EditDialog_Vector::createEdit() {
  AbstractVectorValueType *avvti = dynamic_cast<AbstractVectorValueType*>(avti);

  for (int i=0; i<visiblelineedit; i++) {
    QLabel *label = new QLabel(QString::number(i), this);
    label->setAlignment(Qt::AlignHCenter);
    labellist.append(label);

    Property *property = PropertyFactory::createProperty("dummy", avvti->getElementValueTypeInfo(), "", {});

    QVTIWidget *widget = createWidget(property->getValueTypeInterface());

    widget->getValueTypeInterface()->fromString(vector[i].toLatin1().data());
    connect(widget, &QWidgetInterface::gotFocus, this, &EditDialog_Vector::focusGained);
    linelist.append(widget);
  }
}

void EditDialog_Vector::fillEdit() {
  for (int i=0; i<visiblelineedit; i++) {
    editlayout->addWidget(labellist[i], 0, i);
    editlayout->addWidget(linelist[i]->getWidget(), 1, i);
  }

  linelist[0]->setFocus();
}

/** updating Line edit to show the actual Index and value Pairs
  */
void EditDialog_Vector::updateLine() {
  for (int i=0; i < visiblelineedit; i++) {
    linelist[i]->getValueTypeInterface()->fromString(vector[mostleftedit+i].toStdString());
    labellist[i]->setNum(mostleftedit+i);
  }
}

//save current Line Edit in background vector
void EditDialog_Vector::save() {
  for (int i=0; i < visiblelineedit; i++) {
    vector[mostleftedit + i] = linelist[i]->getValueTypeInterface()->toString().c_str();
  }
}

/** Slot
  * checking content of Line edit and overwrites Vector String when appropiate values
  */
void EditDialog_Vector::apply() {
  save();

  // make a string from background vector
  QString vectorstr("(");
  if (vectorsize > 0) {
    for (int i=0; i<vectorsize; i++) {
      vectorstr += vector[i] + ",";
    }
    // last value has no ','
    vectorstr.chop(1);
  }
  vectorstr += ")";

  // overwrite String with vector String
  avti->fromString(vectorstr.toStdString());

  done(0);
}

/** Slot
  * destroys Widget
  */
void EditDialog_Vector::cancel() {
  done(-1);
}

/** moving Line edit Index acording to @param step while checking boundarys
  */
void EditDialog_Vector::move() {
  save();

  mostleftedit = hscrollbar->value();
  if (position >= mostleftedit && position < mostleftedit+visiblelineedit) {
    linelist[position-mostleftedit]->clearFocus();
    linelist[position-mostleftedit]->setFocus();
  } else if (focusposition != -1) {
    clearHighlighting();
    setHighlight();
  }

  updateLine();
}

void EditDialog_Vector::jump(int index) {
  save();
  if (index > vectorsize-1) index = vectorsize-1;
  if (index < 0) return;

  // which is the lowest visible x index
  mostleftedit = index - roundf(static_cast<float>(visiblelineedit/2.0));

  if (mostleftedit < 0) {
    mostleftedit = 0;
  }

  mostleftedit = std::min(mostleftedit, vectorsize-visiblelineedit);

  int activatex = (index-mostleftedit);

  // no new focusin event if already has focus, need a redo of highlighting if had focus
  long needrepaint = linelist[activatex]->hasFocus();
  linelist[activatex]->setFocus();

  if (needrepaint) {
    focusGained(nullptr);
  }

  updateLine();
  hscrollbar->setSliderPosition(mostleftedit);
}

void EditDialog_Vector::stepLeft() {
  jump(position-1);
}

void EditDialog_Vector::stepRight() {
  jump(position+1);
}

/** Slot
  * setting Line edit Index to jumpmark position while checking boundarys
  * shows Error by inapporate jumpmark
  */
void EditDialog_Vector::jumpTriggered() {
  jump(jumpmark->value());
}

/** Slots
  * calling move with matching parameter if Line edit entrys are correct
  */
void EditDialog_Vector::resize() {
  int size = (vectorlength->value());

  // nothing to do
  if (size == vectorsize) return;
  hscrollbar->setSliderPosition(0);

  QProgressDialog *progress = new QProgressDialog(tr("Please hold the Line currently expanding Vector"), tr("too much stuff"), 1, size-vectorsize, this);
  progress->setWindowModality(Qt::WindowModal);
  progress->setMaximum(size-vectorsize);
  progress->setAutoReset(true);
  QProgressBar *bar = new QProgressBar(progress);
  bar->setMaximum(size-vectorsize);
  bar->setMinimum(1);
  progress->setBar(bar);

  if (size > vectorsize) {
    vector.reserve(size);
    for (int i=0; i < size-vectorsize;i++) {
      vector.append("0"); // set default
      progress->setValue(i+1);
      if (progress->wasCanceled()) {
        QMessageBox::information(this, tr("EditDialog"), tr("Resize canceled. Reset to source string."), QMessageBox::Ok, QMessageBox::Ok);
        reset();
        delete progress;
        return;
      }
    }

  } else if (size < vectorsize) {
    for (int i=0; i < (vectorsize - size); i++) {
      vector.removeLast();
    }
  }
  delete progress;

  vectorsize = size;

  if (focusposition >= vectorsize) {
    clearHighlighting();
  }

  // avoiding empty Lineedits
  int newvisiblelineedit = std::min(visiblelineeditstart, vectorsize);

  if (newvisiblelineedit != visiblelineedit) {
    clearEdit();

    visiblelineedit = newvisiblelineedit;

    createEdit();
    fillEdit();
  }

  updateLine();

  mostleftedit = std::max(size-visiblelineedit, 0);
  hscrollbar->setMaximum(vectorsize-newvisiblelineedit);
  hscrollbar->setHidden((hscrollbar->minimum() == hscrollbar->maximum()));
  hscrollbar->setSliderPosition(mostleftedit);

  jumpmark->setMaximum(vectorsize-1);

}

void EditDialog_Vector::convert() {
  if (!convertable) {
    QMessageBox::information(this, tr("EditDialog"), tr("could not convert value"), QMessageBox::Ok, QMessageBox::Ok);
    return;
  }

  for (int i= 0; i < vectorsize; i++) {
    vector[i] = convertbox->getValueTypeInterface()->toString().c_str();
  }
  updateLine();
}

void EditDialog_Vector::enterPressed() {
  if (jumpmark->hasFocus()) {
    jumpTriggered();
  } else if (editable) {
    if (vectorlength->hasFocus()) {
     resize();
    }
  } else if (convertbox->hasFocus()) {
    convert();
  }
}

void EditDialog_Vector::clearHighlighting() {
  // clear highlighting of old widget
  if (focusposition < 0) return;
  linelist[focusposition]->getWidget()->setAutoFillBackground(true);
  linelist[focusposition]->getWidget()->setPalette(QPalette());
  linelist[focusposition]->clearFocus();

  labellist[focusposition]->setPalette(QPalette());
  // labellist[focusposition]->setFrameStyle(QFrame::NoFrame);
  labellist[focusposition]->setFont(normal);
  focusposition = -1;
}

void EditDialog_Vector::searchFocus() {
  for (int i=0; i<linelist.size(); i++) {
    if (linelist[i]->hasFocus()) {
      focusposition = i;
      return;
    }
  }
  focusposition = -1;
}

void EditDialog_Vector::setHighlight() {
  // highlight scheme
  if (position>=mostleftedit && position<mostleftedit+visiblelineedit) {
    labellist[focusposition]->setFont(bold);
    // labellist[focusposition]->setFrameStyle(QFrame::Panel);
    labellist[focusposition]->setAutoFillBackground(true);
    labellist[focusposition]->setPalette(pal);

    // highlight selected
    if (position>=mostleftedit && position<mostleftedit+visiblelineedit) {
      linelist[focusposition]->getWidget()->setAutoFillBackground(true);
      linelist[focusposition]->getWidget()->setPalette(pal);
    }
  }
}

void EditDialog_Vector::focusGained(QWidgetInterface* /*qwti*/) {
  clearHighlighting();
  searchFocus();
  if (focusposition == -1) {
    position = -1;
    return;
  }
  position = mostleftedit + focusposition;
  setHighlight();
}

void EditDialog_Vector::checkValidate(bool validate) {
  convertable = validate;
}

/** Slot
  * resetting current Vector to old Vector String
  */
void EditDialog_Vector::reset() {
  mostleftedit    = 0;
  hscrollbar->setSliderPosition(mostleftedit);

  // preparing Vector
  vectorsize      = oldvectorsize;
  vectorise(avti->toString().c_str());

  vectorlength->setValue(vectorsize);

  // avoiding empty Lineedits
  visiblelineedit = std::min(visiblelineeditstart, vectorsize);
  hscrollbar->setMaximum(vectorsize-visiblelineedit);
  hscrollbar->setHidden((hscrollbar->minimum() == hscrollbar->maximum()));

  // creating of Lineedits and Label to show current Index and matching Vector values
  clearEdit();
  createEdit();
  fillEdit();
  updateLine();

  jumpmark->setMaximum(vectorsize-1);
}

QDialog* EditDialog_Vector::init(AbstractValueTypeInterface* avti) {
  this->avti = avti;

  // setParent(avti->getOuterWidget());

  fillTitel();

  focusposition = -1;

  QString parentstring = QString(avti->toString().c_str());

  oldvectorsize        = setStartSize(parentstring);

  editable = (vectorsize == 0); // TODO muss in den hint
  lengthbutton->setHidden(editable);
  vectorlength->setHidden(editable);
  lengthbutton->setEnabled(not editable);
  vectorlength->setEnabled(not editable);

  // Flag for validate state of convertbox
  convertable = true;

  AbstractVectorValueType *avvti = dynamic_cast<AbstractVectorValueType*>(avti);

  Property *property = PropertyFactory::createProperty("dummy", avvti->getElementValueTypeInfo(), "", {});

  convertbox = createWidget(property->getValueTypeInterface());
  convertbox->getOuterWidget()->setToolTip(tr("Shortcut: Ctrl + o"));
  convertbox->getOuterWidget()->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
  convertbox->getWidget()->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
  // connect(this->convertbox,    SIGNAL(inputValidated(bool)), this, SLOT(checkValidate(bool)));

  connect(this->filter, &InfileWidget_VectorEditEventFilter::override, convertbox, &QVTIWidget::setFocus);

  fillJump();
  // jumpboxlayout->addWidget(convertbox->getOuterWidget());
  // jumpboxlayout->addStretch();

  reset();
  jump(0);

  return this;
}

void EditDialog_Vector::deinit() {
  clearEdit();

  jumpboxlayout->removeWidget(convertbox);
  delete convertbox;
}

void EditDialog_Vector::help() {
  QMessageBox box;
  box.setText("\nSTRG:\n\nr  :  focus to change size \nj  :  focus to jump \no :  focus to override\n"
              "\n\nALT:\n\nleft    :   move focus to left element \nright  :   move focus to right element\n");
  box.exec();
}
