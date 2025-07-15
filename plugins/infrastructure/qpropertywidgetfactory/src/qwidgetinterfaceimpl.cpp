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

#include <QDebug>
#include <QWidget>
#include <QHBoxLayout>
#include <QLayout>
#include <QMenu>
#include <QList>

#include <plugins/infrastructure/widgetdialog/editdialoginterface.h>

#include "qwidgetinterfaceimpl.h"

QWidgetInterfaceImpl::QWidgetInterfaceImpl(QWidget* widget) : /*QWidget(widget->parent()),*/ widget(widget) {
  this->dialoglist = nullptr;
  this->dialogtoolbutton = nullptr;
  this->dialogpushbutton = nullptr;

  this->filter = new UserInteractionEventFilter(getWidget());

  getWidget()->setFocusPolicy(Qt::StrongFocus);
  getWidget()->installEventFilter(this->filter);
  getOuterWidget()->setLayout(new QHBoxLayout());

  getOuterWidget()->layout()->addWidget(widget);

  wrongvalue = QPalette();
  wrongvalue.setColor(QPalette::Base,   Qt::red);
  wrongvalue.setColor(QPalette::Window, Qt::red);

  connect(filter, &UserInteractionEventFilter::gotFocus, this, [this]() {
    Q_EMIT gotFocus(this);
  });
  connect(filter,                SIGNAL(lostFocus()),                    this, SIGNAL(lostFocus()));
  //     connect(getWidget(),           SIGNAL(needsValidation()),              getWidget(), SLOT(validate()));
  //     connect(getWidget(),           SIGNAL(inputValidated(bool)),           getWidget(), SLOT(highlightValidateState(bool)));

}

void QWidgetInterfaceImpl::resetDefault() {
  const ValueTypeInterfaceHint *hint = getValueTypeInterface()->getHint();
  if (hint->hasEntry("default")) {
    std::string default_value;
    hint->getEntry("default", default_value);
    getValueTypeInterface()->fromString(default_value);
  }
}

#if 0
void QWidgetInterfaceImpl::setRange(const char* const range) { // TODO das tut so noch nicht
  if (range && validator) {
    std::string condition = "INRANGE:" + std::string(range);

    if (validator->conditions) {
      Free(validator->conditions);
    }

    Condition *newcondition = Condition_init(newCondition(), condition.c_str(), (stringParseFunc)parseREAL, compare_REAL_ascending, NULL);
    validator->conditions = newcondition;
  }
}
#endif

QWidgetInterfaceImpl::~QWidgetInterfaceImpl() {
  getWidget()->deleteLater();
}

void QWidgetInterfaceImpl::sendFocus() {
  Q_EMIT gotFocus(this);
}

void QWidgetInterfaceImpl::validate() {
  Q_EMIT inputValidated(validateValue());
}

void QWidgetInterfaceImpl::highlightValidateState(bool isvalid) {
  if (not isvalid) {
    getWidget()->setPalette(wrongvalue);
  } else {
    getWidget()->setPalette(QPalette());
  }
}

void QWidgetInterfaceImpl::addDialog(EditDialogInterface* dialog) {
  QAction *action = new QAction(getOuterWidget());

  // first Dialog
  if (!dialogpushbutton) {
    dialoglist = new QList<EditDialogInterface*>();

    dialogpushbutton = new QPushButton(getOuterWidget());
    dialogpushbutton->setMinimumWidth(60); // reine Optik.
    // if (dialog->getIcon()) dialogpushbutton->setIcon(*dialog->getIcon());
    getOuterWidget()->layout()->addWidget(this->dialogpushbutton);
    action->setParent(dialogpushbutton);
    dialogpushbutton->addAction(action);
    connect(dialogpushbutton, &QPushButton::clicked, action, &QAction::trigger);

  } else if (dialogpushbutton && !dialogtoolbutton) {

    // create toolbutton
    dialogtoolbutton = new QToolButton(getOuterWidget());
    dialogtoolbutton->setMinimumWidth(60); // reine Optik.

    // remove first action from pushbutton
    QAction *firstaction = dialogpushbutton->actions().first();
    dialogpushbutton->removeAction(firstaction);
    firstaction->setParent(dialogtoolbutton);

    // remove gpushbutton from widget
    getOuterWidget()->layout()->removeWidget(dialogpushbutton);
    delete(dialogpushbutton);
    dialogpushbutton = nullptr;

    // create menu for toolbutton
    QMenu *menu = new QMenu(getOuterWidget());
    dialogtoolbutton->setMenu(menu);
    getOuterWidget()->layout()->addWidget(this->dialogtoolbutton);

    // add first action
    dialogtoolbutton->menu()->addAction(firstaction);
    dialogtoolbutton->setDefaultAction(dialogtoolbutton->menu()->actions().at(0));

    // add new
    action->setParent(dialogtoolbutton);
    dialogtoolbutton->menu()->addAction(action);
  } else {
    action->setParent(dialogtoolbutton);
    dialogtoolbutton->menu()->addAction(action);
  }

  if (dialog->getIcon()) {
    action->setIcon(*dialog->getIcon());
  }

  action->setText(dialog->getName());

  connect(action, &QAction::triggered, this, &QWidgetInterfaceImpl::sendFocus);
  connect(action, &QAction::triggered, [dialog, this]() {
    dialog->exec(this->getValueTypeInterface());
  });

  dialoglist->append(dialog);
}
