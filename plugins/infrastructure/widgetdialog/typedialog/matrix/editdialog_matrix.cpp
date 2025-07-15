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

#include <plugins/infrastructure/qpropertywidgetfactory/qvtiwidget.h>

#include "editdialog_matrix.h"
#include "properties/data/propertyfactory.h"

#define X 0
#define Y 1


InfileWidget_MatrixEditEventFilter::InfileWidget_MatrixEditEventFilter(QObject* parent) : QObject(parent) {
}

bool InfileWidget_MatrixEditEventFilter::eventFilter(QObject* obj, QEvent* event) {
  if (event->type() != QEvent::KeyPress) {
    // standard event processing
    return QObject::eventFilter(obj, event);
  }

  // send signal to update descripton
  QKeyEvent *keyevent = static_cast<QKeyEvent*>(event);

  // Combinations with 'Alt'
  if (keyevent->modifiers() == Qt::AltModifier) {
    switch (keyevent->key()) {
      case Qt::Key_Up:
        Q_EMIT altUp();
        return true;

      case Qt::Key_Down:
        Q_EMIT altDown();
        return true;

      case Qt::Key_Left:
        Q_EMIT altLeft();
        return true;

      case Qt::Key_Right:
        Q_EMIT altRight();
        return true;
    }
  }

  // Combinations with 'Control'
  if (keyevent->modifiers() == Qt::ControlModifier) {
    switch (keyevent->key()) {
      case Qt::Key_Return:
        Q_EMIT apply();
        return true;

      case Qt::Key_Enter:
        Q_EMIT apply();
        return true;

      case Qt::Key_I:
        Q_EMIT shortjumpi();
        return true;

      case Qt::Key_J:
        Q_EMIT shortjumpj();
        return true;

      case Qt::Key_N:
        Q_EMIT editn();
        return true;

      case Qt::Key_M:
        Q_EMIT editm();
        return true;

      case Qt::Key_O:
        Q_EMIT override();
        return true;
    }
  }

  switch (keyevent->key()) {
    case Qt::Key_Return:
    case Qt::Key_Enter:
      Q_EMIT triggeredEnter();
      return true;

    case Qt::Key_Left:
      Q_EMIT left();
      return true;

    case Qt::Key_Right:
      Q_EMIT right();
      return true;

    case Qt::Key_Up:
      Q_EMIT up();
      return true;

    case Qt::Key_Down:
      Q_EMIT down();
      return true;
  }

  // standard event processing
  return QObject::eventFilter(obj, event);
}

EditDialog_Matrix::EditDialog_Matrix() : EditDialog() {
  // install filter for handling events
  this->filter        = new InfileWidget_MatrixEditEventFilter(this);
  this->installEventFilter(this->filter);
  icon                = new QIcon(QString(":icons/barretr_Pencil.png"));

  // create Fonts for selected Label
  normal = QFont();
  bold   = QFont();
  bold.setBold(true);

  pal                   = QPalette();
  pal.setColor(QPalette::Base,   QColor(214, 255, 214));
  pal.setColor(QPalette::Window, QColor(214, 255, 214));

  lightPal              = QPalette();
  lightPal.setColor(QPalette::Base,   QColor(224, 235, 224));
  lightPal.setColor(QPalette::Window, QColor(224, 235, 224));

  ilabel = new QLabel("i :", this);
  jlabel = new QLabel("j :", this);
  mlabel = new QLabel("m :", this);
  nlabel = new QLabel("n :", this);
  spacer = new QSpacerItem(1, 0, QSizePolicy::Expanding, QSizePolicy::Minimum);


  // initializing start values
  position[X]           = 0;
  position[Y]           = 0;
  focusposition[X]      = 0;
  focusposition[Y]      = 0;

  visiblelineeditstartx = 10;
  visiblelineeditx      = visiblelineeditstartx;
  visiblelineeditstarty = 10;
  visiblelineedity      = visiblelineeditstarty;

  mostleftedit          = 0;
  mosttopedit           = 0;

  editlayout            = new QGridLayout();

  editableM             = true;
  editableN             = true;

  // Flag for validate state of convertbox
  convertable = true;

  // creating Layouts
  toplevellayout = new QVBoxLayout(this);
  setLayout(toplevellayout);

  vscrolllayout  = new QHBoxLayout();
  hscrolllayout  = new QHBoxLayout();
  midlayout      = new QHBoxLayout();
  jumpboxlayout  = new QHBoxLayout();
  controlslayout = new QHBoxLayout();

  toplevellayout->addLayout(midlayout);
  midlayout->addLayout(editlayout);
  midlayout->addLayout(vscrolllayout);
  toplevellayout->addLayout(hscrolllayout);
  toplevellayout->addLayout(jumpboxlayout);
  toplevellayout->addStretch(1);
  toplevellayout->addLayout(controlslayout);

  // creating Matrix Guis

  jumpbutton = new QPushButton(tr("Jump to"), this);
  jumpbutton->setToolTip(tr("Jump to value in box1 and box2"));
  jumpbutton->setAutoDefault(false);

  jumpmarkx = new QSpinBox(this);
  jumpmarkx->setMinimum(0);
  jumpmarkx->setMaximumWidth(100);
  jumpmarkx->setToolTip(tr("Shortcut: Ctrl + i"));

  jumpmarky = new QSpinBox(this);
  jumpmarky->setMinimum(0);
  jumpmarky->setMaximumWidth(100);
  jumpmarky->setToolTip(tr("Shortcut: Ctrl + j"));

  lengthbutton = new QPushButton("Resize", this);
  lengthbutton->setToolTip(tr("Resize row and column number of the matrix\nchops or adds at the end of row and column and set focus to 0/0"));
  lengthbutton->setAutoDefault(false);
  lengthbutton->setEnabled(false);

  matrixlengthx = new QSpinBox(this);
  matrixlengthx->setMinimum(0);
  matrixlengthx->setMaximum(INT_MAX);
  matrixlengthx->setToolTip(tr("Shortcut: Ctrl + n"));
  matrixlengthx->setEnabled(false);

  matrixlengthy = new QSpinBox(this);
  matrixlengthy->setMinimum(0);
  matrixlengthy->setMaximum(INT_MAX);
  matrixlengthy->setToolTip("Shortcut: Ctrl + m");
  matrixlengthy->setEnabled(false);

  hscrollbar = new QScrollBar(this);
  hscrollbar->setOrientation(Qt::Horizontal);
  hscrollbar->setMinimum(0);
  hscrollbar->setToolTip(tr("Move Matrix"));

  vscrollbar = new QScrollBar(this);
  vscrollbar->setOrientation(Qt::Vertical);
  vscrollbar->setMinimum(0);
  vscrollbar->setToolTip(tr("Move Matrix"));

  convertbutton = new QPushButton("set All", this);
  convertbutton->setToolTip(tr("Set matrix values to value in Box"));
  convertbutton->setAutoDefault(false);

  convertdrop = new QComboBox(this);
  convertdrop->setToolTip(tr("all: override all values\n current row: override all values in current row\n current column: override all values in current column\n current diagonal override all values in current diagonal"));
  convertdrop->addItem(tr("all"));
  convertdrop->addItem(tr("current row"));
  convertdrop->addItem(tr("current column"));
  convertdrop->addItem(tr("current diagonal"));

  connect(this->filter,        SIGNAL(apply()),              this, SLOT(apply()));
  connect(this->filter,        SIGNAL(triggeredEnter()),     this, SLOT(enterPressed()));
  connect(this->filter,        SIGNAL(altUp()),              this, SLOT(stepUp()));
  connect(this->filter,        SIGNAL(altDown()),            this, SLOT(stepDown()));
  connect(this->filter,        SIGNAL(altRight()),           this, SLOT(stepRight()));
  connect(this->filter,        SIGNAL(altLeft()),            this, SLOT(stepLeft()));
  connect(this->filter,        SIGNAL(left()),               this, SLOT(moveSliderLeft()));
  connect(this->filter,        SIGNAL(right()),              this, SLOT(moveSliderRight()));
  connect(this->filter,        SIGNAL(up()),                 this, SLOT(moveSliderUp()));
  connect(this->filter,        SIGNAL(down()),               this, SLOT(moveSliderDown()));
  connect(this->filter,        SIGNAL(shortjumpi()),         jumpmarkx, SLOT(setFocus()));
  connect(this->filter,        SIGNAL(shortjumpj()),         jumpmarky, SLOT(setFocus()));
  connect(this->filter,        SIGNAL(editn()),              matrixlengthx, SLOT(setFocus()));
  connect(this->filter,        SIGNAL(editm()),              matrixlengthy, SLOT(setFocus()));
  connect(this->hscrollbar,    SIGNAL(valueChanged(int)),    this, SLOT(moveX()));
  connect(this->vscrollbar,    SIGNAL(valueChanged(int)),    this, SLOT(moveY()));
  connect(this->lengthbutton,  SIGNAL(clicked()),            this, SLOT(resize()));
  connect(this->jumpbutton,    SIGNAL(clicked()),            this, SLOT(jumpTriggered()));
  connect(this->convertbutton, SIGNAL(clicked()),            this, SLOT(convert()));

  // filling Layouts
  fillScroll();
  fillControl();

  setAccessibleName("Edit");
}

// Destructor
EditDialog_Matrix::~EditDialog_Matrix() {

}

// scroll up and down
void EditDialog_Matrix::wheelEvent(QWheelEvent* event) {
  QPoint numPixels = event->pixelDelta();
  QPoint numDegrees = event->angleDelta();

  if (!numPixels.isNull()) {
    vscrollbar->setSliderPosition(vscrollbar->value()-numPixels.y());
  } else if (!numDegrees.isNull()) {
    QPoint numSteps = numDegrees / 120;
    vscrollbar->setSliderPosition(vscrollbar->value()-numSteps.y());
  }

  event->accept();
}

// fill titel Layout
void EditDialog_Matrix::fillTitel() {
  QString  title;
  if (avti->getHint()->hasEntry("label")) {
    title += QString::fromStdString(avti->getHint()->getEntry("label")) + ": ";
  }

  AbstractMatrixValueType *amvti = dynamic_cast<AbstractMatrixValueType*>(avti);
  title += "matrix of type ";
  title += demangle(amvti->getElementValueTypeInfo().name()).c_str();
  this->setWindowTitle(title);
}

// fill edit Layout
void EditDialog_Matrix::fillEdit() {
  // y Label
  for (int y = 0; y < visiblelineedity; y++) {
    editlayout->addWidget(labellisty[y], y+1, 0);
  }

  // x Label and Widgets
  for (int x = 0; x < visiblelineeditx; x++) {
    editlayout->addWidget(labellistx[x], 0, x+1);

    for (int y = 0; y < visiblelineedity; y++) {
      editlayout->addWidget(linelist[y].value(x)->getWidget(), y+1, x+1);
    }
  }
}

// fill scroll Layout
void EditDialog_Matrix::fillScroll() {
  hscrolllayout->addWidget(hscrollbar);
  vscrolllayout->addWidget(vscrollbar);
}

// fill jump Layout
void EditDialog_Matrix::fillJump() {
  jumpboxlayout->addWidget(jumpbutton);
  jumpboxlayout->addWidget(ilabel);
  jumpboxlayout->addWidget(jumpmarkx);

  jumpboxlayout->addWidget(jlabel);
  jumpboxlayout->addWidget(jumpmarky);
  jumpboxlayout->addWidget(lengthbutton);

  jumpboxlayout->addWidget(mlabel);
  jumpboxlayout->addWidget(matrixlengthx);

  jumpboxlayout->addWidget(nlabel);
  jumpboxlayout->addWidget(matrixlengthy);

  jumpboxlayout->addWidget(convertbutton);
  jumpboxlayout->addWidget(convertdrop);
  jumpboxlayout->addWidget(convertbox->getOuterWidget());
  jumpboxlayout->addItem(spacer);
}

// fill control Layout
void EditDialog_Matrix::fillControl() {
  QPushButton *helpbutton = new QPushButton(QIcon(":icons/help-browser.png"), "", this);
  helpbutton->setToolTip(tr("Shortcut summary"));
  helpbutton->setAutoDefault(false);
  controlslayout->addWidget(helpbutton);

  QSpacerItem *space = new QSpacerItem(0, 0, QSizePolicy::Expanding);
  controlslayout->addSpacerItem(space);

  QPushButton *okbutton = new QPushButton(tr("Ok"), this);
  okbutton->setToolTip(tr("Apply your changes and leave editor"));
  okbutton->setAutoDefault(false);
  controlslayout->addWidget(okbutton);

  QPushButton *resetbutton = new QPushButton(tr("Reset"), this);
  resetbutton->setToolTip(tr("Reset your changes"));
  resetbutton->setAutoDefault(false);
  controlslayout->addWidget(resetbutton);

  QPushButton *cancelbutton = new QPushButton(tr("Cancel"), this);
  cancelbutton->setFocus();
  cancelbutton->setToolTip(tr("Reset your changes and leave editor"));
  cancelbutton->setAutoDefault(false);
  controlslayout->addWidget(cancelbutton);

  connect(helpbutton,    SIGNAL(clicked()),            this, SLOT(help()));
  connect(okbutton,      SIGNAL(clicked()),            this, SLOT(apply()));
  connect(resetbutton,   SIGNAL(clicked()),            this, SLOT(reset()));
  connect(cancelbutton,  SIGNAL(clicked()),            this, SLOT(cancel()));
}

// calculates number of Vectors by counting ')' and ','
void EditDialog_Matrix::setStartSize(const QString& parentstring) {
  if (parentstring.size() == 0) {
    oldmatrixsizey = 0;
    oldmatrixsizex = 0;
  } else {
    oldmatrixsizey = (parentstring.count(")"));
    oldmatrixsizex = ((parentstring.count(",")-(oldmatrixsizey-1))/oldmatrixsizey)+1;
  }
}

// create Label and Lineedits
void EditDialog_Matrix::createEdit() {
  int linelistcount = linelist.count();

  // append missing QLists
  for (int y = 0; y < visiblelineedity-linelistcount; y++) {
    linelist.append(QList<QVTIWidget*>());
  }

  // insert created widget in QList and connects signals
  for (int y = 0; y < visiblelineedity; y++) {
    for (int x = 0; x < visiblelineeditx; x++) {
      AbstractMatrixValueType *amvti = dynamic_cast<AbstractMatrixValueType*>(avti);

      Property *property = PropertyFactory::createProperty("dummy", amvti->getElementValueTypeInfo(), "", {});

      QVTIWidget *widget = createWidget(property->getValueTypeInterface());

      widget->getValueTypeInterface()->fromString(matrix[y].value(x).toStdString());
      // widget->getWidget()->installEventFilter(this->filter);

      connect(widget, &QWidgetInterface::gotFocus, this, &EditDialog_Matrix::focusGained);
      linelist[y].append(widget);
    }
  }

  // create and insert Label
  for (int x = 0; x < visiblelineeditx; x++) {
    QLabel *label = new QLabel(QString::number(x), this);
    label->setAlignment(Qt::AlignHCenter);
    labellistx.append(label);
  }
  for (int y = 0; y < visiblelineedity; y++) {
    QLabel *label = new QLabel(QString::number(y), this);
    label->setAlignment(Qt::AlignVCenter);
    labellisty.append(label);
  }
}

// filling of Matrix with all values of @param str
bool EditDialog_Matrix::matrixise(const QString& parentstring) {
  matrix.clear();

  int            i = 0;
  QList<QString> templist;

  // store Strings with value (* in temporary List
  templist = parentstring.split(')');

  // match first Vector with others
  templist[X].prepend(",");

  // for each element in templist remove beginning '(' and store splitted List in background matrix
  int sizecount = templist.size();
  int splitsize = templist[0].count(',') - 1;

  while (sizecount-1 > i) {
    templist[i].remove(0, 2);

    if (templist[i].count(',') != splitsize) {
      QMessageBox::warning(this, tr("Matrix dialog"), tr("Matrix is corrupt. Rows do not have the same amount of elements."), QMessageBox::Ok, QMessageBox::Ok);
      return false;
    } else {
      matrix.append(templist[i].split(','));
    }

    i++;
  }

  // correction of display by reset
  if (visiblelineeditx > matrixsizex) {
    visiblelineeditx = matrixsizex;
  }

  if (visiblelineedity > matrixsizey) {
    visiblelineedity = matrixsizey;
  }

  return true;
}

// moves values in horizontal direction
void EditDialog_Matrix::moveX() {
  save();

  mostleftedit = hscrollbar->value();

  if ((position[Y] >= mosttopedit) && (position[Y] < mosttopedit+visiblelineedity) && (position[X] >= mostleftedit) && (position[X] < mostleftedit+visiblelineeditx)) {
    linelist[position[Y]-mosttopedit].value(position[X]-mostleftedit)->clearFocus();
    linelist[position[Y]-mosttopedit].value(position[X]-mostleftedit)->setFocus();
  } else {
    clearHighlighting();
    linelist[focusposition[Y]].value(focusposition[X])->clearFocus();

    if ((position[Y] >= mosttopedit) && (position[Y] < mosttopedit+visiblelineedity)) {
      focusposition[Y] = position[Y]-mosttopedit;
    }

    if ((position[X] >= mostleftedit) && (position[X] < mostleftedit+visiblelineeditx)) {
      focusposition[X] = position[X]-mostleftedit;
    }

    setHighlight();
  }

  updateLine();
}

// moves values in vertical direction
void EditDialog_Matrix::moveY() {
  save();

  mosttopedit = vscrollbar->value();

  if ((position[Y] >= mosttopedit) && (position[Y] < mosttopedit+visiblelineedity) && (position[X] >= mostleftedit) && (position[X] < mostleftedit+visiblelineeditx)) {
    linelist[position[Y]-mosttopedit].value(position[X]-mostleftedit)->clearFocus();
    linelist[position[Y]-mosttopedit].value(position[X]-mostleftedit)->setFocus();
  } else {
    clearHighlighting();
    linelist[focusposition[Y]].value(focusposition[X])->clearFocus();

    if ((position[Y] >= mosttopedit) && (position[Y] < mosttopedit+visiblelineedity)) {
      focusposition[Y] = position[Y]-mosttopedit;
    }

    if ((position[X] >= mostleftedit) && (position[X] < mostleftedit+visiblelineeditx)) {
      focusposition[X] = position[X]-mostleftedit;
    }

    setHighlight();
  }

  updateLine();
}

// clear highlighting of old focus
void EditDialog_Matrix::clearHighlighting() {
  // clear highlighting of old widget

  if (focusposition[X] < visiblelineeditx) {
    for (int y = 0; y < visiblelineedity; y++) {
      linelist[y].value(focusposition[X])->getWidget()->setAutoFillBackground(true);
      linelist[y].value(focusposition[X])->getWidget()->setPalette(QPalette());
    }
    labellistx[focusposition[X]]->setPalette(QPalette());
    labellistx[focusposition[X]]->setFrameStyle(QFrame::NoFrame);
    labellistx[focusposition[X]]->setFont(normal);
  }

  if (focusposition[Y] < visiblelineedity) {
    for (int x = 0; x < visiblelineeditx; x++) {
      linelist[focusposition[Y]].value(x)->getWidget()->setAutoFillBackground(true);
      linelist[focusposition[Y]].value(x)->getWidget()->setPalette(QPalette());
    }
    labellisty[focusposition[Y]]->setPalette(QPalette());
    labellisty[focusposition[Y]]->setFrameStyle(QFrame::NoFrame);
    labellisty[focusposition[Y]]->setFont(normal);
  }
}

void EditDialog_Matrix::stepUp() {
  if (position[Y]-1 < 0) {
    return;
  }

  jump(position[X], position[Y]-1);
}

void EditDialog_Matrix::stepDown() {
  if (position[Y]+1 >= matrixsizey) {
    return;
  }

  jump(position[X], position[Y]+1);
}

void EditDialog_Matrix::stepRight() {
  if (position[X]+1 >= matrixsizex) {
    return;
  }

  jump(position[X]+1, position[Y]);
}

void EditDialog_Matrix::stepLeft() {
  if (position[X]-1 < 0) {
    return;
  }

  jump(position[X]-1, position[Y]);
}

void EditDialog_Matrix::jumpTriggered() {
  clearHighlighting();
  jump(jumpmarkx->value(), jumpmarky->value());
}

// moves choosen value in middle and set focus
void EditDialog_Matrix::jump(int targetx, int targety) {
  save();

  // ensure everything is inside matrix
  if (targetx < 0) {
    targetx = 0;
  } else if (targetx >= matrixsizex) {
    targetx = matrixsizex-1;
  }
  if (jumpmarkx->value() != targetx) {
    jumpmarkx->setValue(targetx);
  }

  if (targety < 0) {
    targety = 0;
  } else if (targety >= matrixsizey) {
    targety = matrixsizey-1;
  }
  if (jumpmarky->value() != targety) {
    jumpmarky->setValue(targety);
  }

  // now have new cordinates
  position[X] = targetx;
  position[Y] = targety;

  // which is the lowest visible x index
  mostleftedit = targetx - roundf(static_cast<float>(visiblelineeditx/2.0));

  if (mostleftedit < 0) {
    mostleftedit = 0;
  }

  if (mostleftedit + visiblelineeditx > matrixsizex) {
    mostleftedit = (matrixsizex-visiblelineeditx);
  }

  mosttopedit = targety - roundf(static_cast<float>(visiblelineedity/2.0));

  if (mosttopedit < 0) {
    mosttopedit = 0;
  }

  if (mosttopedit + visiblelineedity > matrixsizey) {
    mosttopedit = (matrixsizey-visiblelineedity);
  }

  int activatex = (targetx-mostleftedit);
  int activatey = (targety-mosttopedit);

  // no new focusin event if already has focus, need a redo of highlighting if had focus
  long needrepaint = linelist[activatey].value(activatex)->hasFocus();
  linelist[activatey].value(activatex)->setFocus();

  if (needrepaint) {
    focusGained(nullptr);
  }

  updateLine();
  hscrollbar->setSliderPosition(mostleftedit);
  vscrollbar->setSliderPosition(mosttopedit);
}

// overrides paarent string and emits signal
void EditDialog_Matrix::apply() {
  save();

  QString matrixstr("[");

  for (int y = 0; y < matrixsizey; y++) {
    matrixstr += "(";
    if (matrixsizex > 0) {
      for (int x = 0; x < matrixsizex-1; x++) {
        matrixstr += (matrix[y].value(x));
        matrixstr += ",";
      }

      // last value has no ','
      matrixstr += matrix[y].value(matrixsizex-1);
    }
    matrixstr += ")";

    matrixstr += ",";
  }
  matrixstr.chop(1);
  matrixstr += "]";

  // set matrix to modified one
  avti->fromString(matrixstr.toStdString());

  done(0);
}

void EditDialog_Matrix::cancel() {
  done(0);
}

// change all values choosen by combobox according to selected value
void EditDialog_Matrix::convert() {
  if (!convertable) {
    QMessageBox::warning(this, tr("Matrix dialog"), tr("could not convert value"));
    return;
  }

  // create Progress bar
  QProgressDialog *progress = new QProgressDialog(tr("Please hold the Line currently converting Matrix"), tr("too much stuff"), 1, 1, this);
  progress->setWindowModality(Qt::WindowModal);
  progress->setMinimumDuration(1);
  progress->setMaximum(matrixsizex*matrixsizey);
  progress->setAutoReset(true);
  QProgressBar *bar = new QProgressBar(progress);
  bar->setMaximum(matrixsizex*matrixsizey);
  bar->setMinimum(1);
  progress->setBar(bar);

  int progresscount = 0;

  switch (convertdrop->currentIndex()) {

    case 0:
      for (int y = 0; y < matrixsizey; y++) {
        progress->setValue(progresscount);
        for (int x = 0; x < matrixsizex; x++) {
          matrix[y].replace(x, convertbox->getValueTypeInterface()->toString().c_str());
          progresscount++;

          if (progress->wasCanceled()) {
            QMessageBox::information(this, tr("Matrix Dialog"), tr("Resize canceled. Reset to source string."), QMessageBox::Ok, QMessageBox::Ok);
            reset();
            delete progress;
            return;
          }
        }
      }
      break;

    case 2:

      if (position[X] <= matrixsizex-1) {
        for (int y = 0; y < matrixsizey; y++) {
          matrix[y].replace(position[X], convertbox->getValueTypeInterface()->toString().c_str());
        }
      }

      break;

    case 1:

      if (position[Y] <= matrixsizey-1) {
        for (int x = 0; x < matrixsizex; x++) {
          matrix[position[Y]].replace(x, convertbox->getValueTypeInterface()->toString().c_str());
        }
      }

      break;

    case 3:

      if ((position[Y] <= matrixsizey-1) && (position[X] <= matrixsizex-1)) {
        int i = position[X];
        int j = position[Y];

        while (i < matrixsizex-1 && j < matrixsizey-1) {
          matrix[j].replace(i, convertbox->getValueTypeInterface()->toString().c_str());
          i++;
          j++;
        }

        while (i >= 0 && j >= 0) {
          matrix[j].replace(i, convertbox->getValueTypeInterface()->toString().c_str());
          i--;
          j--;
        }
      }

      break;
  }

  updateLine();
  delete progress;
}

// change size of matrix
void EditDialog_Matrix::resize() {
  int sizex = matrixsizex;

  if (editableN) {
    sizex = (matrixlengthx->value());
  }

  int sizey = matrixsizey;

  if (editableM) {
    sizey = (matrixlengthy->value());
  }

  if ((sizex <= 0) || (sizey <= 0)) {
    return;
  }

  jump(0, 0);

  clearEdit();

  // create progress bar
  QProgressDialog *progress = new QProgressDialog(tr("Please hold the Line currently expanding Matrix"), tr("too much stuff"), 1, 1, this);
  progress->setWindowModality(Qt::WindowModal);
  progress->setMinimumDuration(1);
  progress->setMaximum(sizey*sizex);
  progress->setAutoReset(true);
  QProgressBar *bar = new QProgressBar(progress);
  bar->setMaximum(sizey*sizex);
  bar->setMinimum(1);
  progress->setBar(bar);
  int progresscount = matrixsizey*matrixsizex;

  // expand in vertical direction
  if (sizey > matrixsizey) {
    for (int y = 0; y < sizey-matrixsizey; y++) {
      matrix.append(QList<QString>());
      for (int x = 0; x < matrixsizex; x++) {
        matrix[matrixsizey+y].insert(0, "0");
        progresscount++;
      }

      // cancel button in Progressbar pressedvectorline->text()
      if (progress->wasCanceled()) {
        QMessageBox::information(this, tr("Matrix Dialog"), tr("Resize canceled. Reset to source string."), QMessageBox::Ok, QMessageBox::Ok);
        reset();
        delete progress;
        return;
      }

      progress->setValue(progresscount);
    }

    // if nessecary increase rows
    if ((sizey > visiblelineedity) && (visiblelineedity < visiblelineeditstarty)) {
      if (sizey < visiblelineeditstarty) {
        visiblelineedity = sizey;
      } else {
        visiblelineedity = visiblelineeditstarty;
      }
    }

    // chop in vertical drirection
  } else if (sizey < matrixsizey) {
    for (int y = 0; y < (matrixsizey - sizey); y++) {
      matrix.removeLast();
    }

    // if necessary reduce rows
    if (sizey < visiblelineedity) {
      mosttopedit      = 0;
      visiblelineedity = sizey;
    }
  }

  // expand in horizontal direction
  if (sizex > matrixsizex) {
    for (int y = 0; y < sizey; y++) {
      for (int x = 0; x < sizex-matrixsizex; x++) {
        matrix[y].append("0");
        progresscount++;
      }

      if (progress->wasCanceled()) {
        QMessageBox::information(this, tr("Matrix Dialog"), tr("Resize canceled. Reset to source string."), QMessageBox::Ok, QMessageBox::Ok);
        reset();
        delete progress;
        return;
      }

      progress->setValue(progresscount);
    }

    // if necessary increase columns
    if ((sizex > visiblelineeditx) && (visiblelineeditx < visiblelineeditstartx)) {
      if (sizex < visiblelineeditstartx) {
        visiblelineeditx = sizex;
      } else {
        visiblelineeditx = visiblelineeditstartx;
      }
    }

    // chop in horizontal direction
  } else if (sizex < matrixsizex) {
    for (int y = 0; y < sizey; y++) {
      for (int x = 0; x < (matrixsizex - sizex); x++) {
        matrix[y].removeLast();
      }
    }

    // if nessecary reduce columns
    if (sizex < visiblelineeditx) {
      mostleftedit     = 0;
      visiblelineeditx = sizex;
    }
  }

  matrixsizex = sizex;
  matrixsizey = sizey;

  createEdit();
  fillEdit();

  hscrollbar->setSliderPosition(0);
  vscrollbar->setSliderPosition(0);

  hscrollbar->setMaximum(sizex-visiblelineeditx);
  vscrollbar->setMaximum(sizey-visiblelineedity);

  if (vscrollbar->minimum() == vscrollbar->maximum()) {
    vscrollbar->hide();
  } else {
    vscrollbar->show();
  }

  if (hscrollbar->minimum() == hscrollbar->maximum()) {
    hscrollbar->hide();
  } else {
    hscrollbar->show();
  }

  linelist[0].value(0)->setFocus();
  updateLine();

  delete progress;
}

void EditDialog_Matrix::updateLine() {
  for (int y = 0; y < visiblelineedity; y++) {
    labellisty[y]->setNum(y+mosttopedit);
    for (int x = 0; x < visiblelineeditx; x++) {
      linelist[y].value(x)->getValueTypeInterface()->fromString(matrix[y+mosttopedit].value(x+mostleftedit).toLatin1().toStdString());
      labellistx[x]->setNum(x+mostleftedit);

    }
  }
}

// safe current Line Edit in background Matrix
void EditDialog_Matrix::save() {
  for (int y = 0; y < visiblelineedity; y++) {
    for (int x = 0; x < visiblelineeditx; x++) {
      matrix[y+mosttopedit].replace(x+mostleftedit,linelist[y].value(x)->getValueTypeInterface()->toString().c_str());
    }
  }
}

/** @brief clear all Widgets in editLayout
  */
void EditDialog_Matrix::clearEdit() {
  for (int y = 0; y < linelist.count(); y++) {
    for (int x = 0; x < linelist[y].count(); x++) {
      editlayout->removeWidget(linelist[y].value(x)->getWidget());
      delete linelist[y].value(x);
    }
    linelist[y].clear();
  }
  for (int x = 0; x < labellistx.count(); x++) {
    editlayout->removeWidget(labellistx[x]);
    delete labellistx[x];
  }
  for (int y = 0; y < labellisty.count(); y++) {
    editlayout->removeWidget(labellisty[y]);
    delete labellisty[y];
  }
  labellistx.clear();
  labellisty.clear();
}

void EditDialog_Matrix::moveSliderLeft() {
  hscrollbar->setSliderPosition(hscrollbar->sliderPosition()-1);
}

void EditDialog_Matrix::moveSliderRight() {
  hscrollbar->setSliderPosition(hscrollbar->sliderPosition()+1);
}

void EditDialog_Matrix::moveSliderUp() {
  vscrollbar->setSliderPosition(vscrollbar->sliderPosition()-1);
}

void EditDialog_Matrix::moveSliderDown() {
  vscrollbar->setSliderPosition(vscrollbar->sliderPosition()+1);
}

void EditDialog_Matrix::enterPressed() {
  if ((editableN && matrixlengthx->hasFocus()) || (editableM && matrixlengthy->hasFocus())) {
    resize();
  } else if (jumpmarky->hasFocus() || jumpmarkx->hasFocus()) {
    jumpTriggered();
  } else if (convertbox->hasFocus()) {
    convert();
  }
}

void EditDialog_Matrix::focusGained(QWidgetInterface* /*qwti*/) {
  clearHighlighting();
  searchFocus();
  position[X] = focusposition[X]+mostleftedit;
  position[Y] = focusposition[Y]+mosttopedit;

  setHighlight();
}

// highlight sceeme
void EditDialog_Matrix::setHighlight() {
  if ((position[X] >= mostleftedit) && (position[X] < mostleftedit+visiblelineeditx)) {
    labellistx[focusposition[X]]->setFont(bold);
    labellistx[focusposition[X]]->setFrameStyle(QFrame::Panel);
    labellistx[focusposition[X]]->setAutoFillBackground(true);
    labellistx[focusposition[X]]->setPalette(pal);

    for (int y = 0; y < visiblelineedity; y++) {
      linelist[y].value(focusposition[X])->getWidget()->setAutoFillBackground(true);
      linelist[y].value(focusposition[X])->getWidget()->setPalette(lightPal);
    }
  }

  if ((position[Y] >= mosttopedit) && (position[Y] < mosttopedit+visiblelineedity)) {

    labellisty[focusposition[Y]]->setFont(bold);
    labellisty[focusposition[Y]]->setFrameStyle(QFrame::Panel);
    labellisty[focusposition[Y]]->setAutoFillBackground(true);
    labellisty[focusposition[Y]]->setPalette(pal);

    for (int x = 0; x < linelist[0].count(); x++) {
      linelist[focusposition[Y]].value(x)->getWidget()->setAutoFillBackground(true);
      linelist[focusposition[Y]].value(x)->getWidget()->setPalette(lightPal);
    }

    // highlight selected
    if ((position[X] >= mostleftedit) && (position[X] < mostleftedit+visiblelineeditx)) {
      linelist[focusposition[Y]].value(focusposition[X])->getWidget()->setAutoFillBackground(true);
      linelist[focusposition[Y]].value(focusposition[X])->getWidget()->setPalette(pal);
    }
  }
}

void EditDialog_Matrix::searchFocus() {
  for (int x = 0; x < visiblelineeditx; x++) {
    for (int y = 0; y < visiblelineedity; y++) {
      if (linelist[y].value(x)->hasFocus()) {
        focusposition[X] = x;
        focusposition[Y] = y;
        return;
      }
    }
  }
}

// reset all values
void EditDialog_Matrix::reset() {
  // reset to start value
  hscrollbar->setSliderPosition(0);
  vscrollbar->setSliderPosition(0);
  position[X]      = 0;
  position[Y]      = 0;
  focusposition[X] = 0;
  focusposition[Y] = 0;
  mostleftedit     = 0;
  mosttopedit      = 0;
  matrixsizex      = oldmatrixsizex;
  matrixsizey      = oldmatrixsizey;

  matrixlengthx->setValue(matrixsizex);
  matrixlengthy->setValue(matrixsizey);

  // preparing Matrix
  QString parentstring = QString::fromStdString(avti->toString());

  // remove beginning '[' and closing ']'
  parentstring.remove(0,1);
  parentstring.chop(1);

  // handels "[]"
  if (parentstring.isEmpty()) {
    parentstring.append("()");
  }
  validate              = matrixise(parentstring);

  // avoiding empty Lineedits
  visiblelineeditstartx = 10;
  visiblelineeditx      = std::min(visiblelineeditstartx, matrixsizex);
  visiblelineeditstarty = 10;
  visiblelineedity      = std::min(visiblelineeditstarty, matrixsizey);

  // creating of Lineedits and Label to show current Index and matching Matrix values
  clearEdit();
  createEdit();
  fillEdit();
  updateLine();

  linelist[0].value(0)->setFocus();

  hscrollbar->setMaximum(matrixsizex-visiblelineeditx);
  hscrollbar->setHidden((hscrollbar->minimum() == hscrollbar->maximum()));
  vscrollbar->setMaximum(matrixsizey-visiblelineedity);
  vscrollbar->setHidden((vscrollbar->minimum() == vscrollbar->maximum()));

  jumpmarkx->setMaximum(matrixsizex-1);
  jumpmarky->setMaximum(matrixsizey-1);
}

QDialog* EditDialog_Matrix::init(AbstractValueTypeInterface* avti) {
  this->avti = avti;

  fillTitel();

  QString parentstring = QString::fromStdString(avti->toString());

  setStartSize(parentstring);

#if 0
  char *dimensionm = Keytype_getDimensionFromVariable(parent->getKeyType(), 0);

  if (strlen(dimensionm) > 0) {
    editableM = false;
  }

  Free(dimensionm);

  char *dimensionn = Keytype_getDimensionFromVariable(parent->getKeyType(), 1);

  if (strlen(dimensionn) > 0) {
    editableN = false;
  }

  Free(dimensionn);
#endif

  // lock resize funktion
  lengthbutton->setEnabled(editableM || editableN);
  matrixlengthx->setEnabled(editableM);
  matrixlengthy->setEnabled(editableN);

  // create convertbox
  AbstractMatrixValueType *amvti = dynamic_cast<AbstractMatrixValueType*>(avti);

  Property *property = PropertyFactory::createProperty("dummy", amvti->getElementValueTypeInfo(), "", {});

  convertbox = createWidget(property->getValueTypeInterface());
  convertbox->getOuterWidget()->setToolTip(tr("Shortcut: Ctrl + o"));
  convertbox->getOuterWidget()->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
  // connect(this->convertbox,    SIGNAL(inputValidated(bool)), this, SLOT(checkValidate(bool)));
  connect(this->filter,        SIGNAL(override()),           convertbox, SLOT(setFocus()));

  fillJump();

  reset();

  return this;
}

void EditDialog_Matrix::deinit() {
  clearEdit();
  jumpboxlayout->removeWidget(convertbox->getWidget());
  jumpboxlayout->removeItem(spacer);
  delete(convertbox);
}

void EditDialog_Matrix::help() {
  QMessageBox box(this);
  box.setText(tr("When slidebar has focus:\nleft    :   move slider left \nright  :   move slider right\nup     :   move slider up\ndown :   move slider down \n"
                 "\n\nSTRG:\n\nn  :  focus to change row size\nm :  focus to change column size \ni   :  focus to jump i index\nj   :  focus to jump j index\no  :  focus to override\n"
                 "\n\nALT:\n\nleft    :   move focus to left element \nright  :   move focus to right element\nup     :   move focus to element above\ndown :   move focus to element under"));
  box.exec();
}

void EditDialog_Matrix::checkValidate(bool validate) {
  convertable = validate;
}
