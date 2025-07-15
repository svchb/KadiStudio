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

#include <QLabel>
#include <QMessageBox>

#include "editdialog_vectorfill.h"

EditDialog_VectorFill::EditDialog_VectorFill() : EditDialog() {

  icon = new QIcon(QString(":icons/barretr_Pencil.png"));

  createUI();

  setAccessibleName("Fill");
}

EditDialog_VectorFill::~EditDialog_VectorFill() {
}

void EditDialog_VectorFill::createUI() {
  toplevellayout = new QGridLayout(this);
  setLayout(toplevellayout);

  vectorlength = new QSpinBox(this);
  vectorlength->setToolTip(tr("the number of phases to generate"));
  vectorlength->setMinimum(1);
  vectorlength->setMaximum(INT_MAX);

  prefix = new QLineEdit(this);
  prefix->setToolTip(tr("the prefix assigned to the sequential number of phases"));
  toplevellayout->addWidget(prefix, 1, 1, 1, 1);

  toplevellayout->addWidget(new QLabel(tr("Vector Size")), 0, 0, 1, 1);
  toplevellayout->addWidget(vectorlength, 0, 1, 1, 1);

  toplevellayout->addWidget(new QLabel(tr("Value for all elements")), 1, 0, 1, 1);

  QHBoxLayout *buttonlayout = new QHBoxLayout();

  okbutton     = new QPushButton(tr("Ok"),     this);
  okbutton->setToolTip(tr("Apply changes and leave editor"));

  cancelbutton = new QPushButton(tr("Cancel"), this);
  cancelbutton->setToolTip(tr("Leave editor"));

  helpbutton   = new QPushButton(QIcon(":icons/help-browser.png"), "", this);
  helpbutton->setToolTip(tr("Shortcut summary"));
  helpbutton->setAutoDefault(false);

  buttonlayout->addWidget(helpbutton);
  buttonlayout->addSpacerItem(new QSpacerItem(20, 20, QSizePolicy::Expanding));
  buttonlayout->addWidget(okbutton);
  buttonlayout->addWidget(cancelbutton);

  connect(helpbutton,   SIGNAL(clicked()),        this, SLOT(help()));
  connect(okbutton,     SIGNAL(clicked()),        this, SLOT(apply()));
  connect(cancelbutton, SIGNAL(clicked()),        this, SLOT(cancel()));

  toplevellayout->addLayout(buttonlayout,2,0,1,2);
}

QDialog* EditDialog_VectorFill::init(AbstractValueTypeInterface* avti) {
  this->avti = avti;

  prefix->setText("grain_");

  //Getting the Data from the Widgets/Infile
  vectorlength->setValue(setStartSize());

  return this;
}

void EditDialog_VectorFill::deinit() {
}

//returns number of ',' in @param str
int EditDialog_VectorFill::setStartSize() {
  QString parentstring = QString(avti->toString().c_str());
  if (parentstring.size() == 0) {
    return 0;
  } else {
    return (parentstring.count(",")+1);
  }
}

/*Slot
  checking content of Line edit and overwrites Vector String with appropiate values
*/
void EditDialog_VectorFill::apply() {
  QString pref = prefix->text();

  int vectorsize = vectorlength->value();
  QString vectorstr("(");

  if (vectorsize > 0) {
    for (int i=1; i < vectorsize; i++) {
      vectorstr += pref + QString::number(i) + ",";
    }
    vectorstr += pref + QString::number(vectorsize); // last value has no ','
  }
  vectorstr += ")";

  // overwrite String with vector String
  avti->fromString(vectorstr.toLatin1().toStdString());

  done(0);
}

/*Slot
  destroys Widget
  */
void EditDialog_VectorFill::cancel() {
  done(-1);
}

/*Slot
  help message
  */
void EditDialog_VectorFill::help() {
  QMessageBox box;
  box.setText(tr("generates a Vector of the given size with sequential numbered names\n\nExample: a vector size of 3 and a prefix of 'test' generates\n\n'(test1,test2,test3)'"));
  box.exec();
}
