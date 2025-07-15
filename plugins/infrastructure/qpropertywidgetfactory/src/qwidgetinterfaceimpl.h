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

#pragma once

#include <QWidget>
#include <QPalette>
#include <QList>
#include <QDialog>
#include <QPushButton>
#include <QToolButton>

#include "properties/data/valuetypeinterface/abstractvaluetypeinterface.h"
#include "../userinteractioneventfilter.h"
#include "../qwidgetinterface.h"

class AbstractValueTypeInterface;
class EditDialogInterface;

class QWidgetInterfaceImpl : public QWidgetInterface {
    Q_OBJECT

  public:

    QWidgetInterfaceImpl(QWidget* widget);
    virtual ~QWidgetInterfaceImpl();

    const QWidget* getOuterWidget() const override {
      return this;
    }
    QWidget* getOuterWidget() override {
      return this;
    }
    QWidget* getWidget() const override {
      return widget;
    }

    virtual AbstractValueTypeInterface* getValueTypeInterface() const = 0; // Kann das raus?

#if 0
    void setRange(const char* const range); // TODO das tut so noch nicht
#endif

    void addDialog(EditDialogInterface* dialog) override;
    const QList<EditDialogInterface*>& getDialogList() override {
      return *dialoglist;
    }

  protected:

    QList<EditDialogInterface*> *dialoglist;
    QToolButton     *dialogtoolbutton;
    QPushButton     *dialogpushbutton;

  public Q_SLOTS:

    void resetDefault() override;

    void setFocus() override {
      getWidget()->setFocus();
    }
    void clearFocus() override {
      getWidget()->clearFocus();
    }
    bool hasFocus() override {
      return getWidget()->hasFocus();
    }

  protected Q_SLOTS:

    void sendFocus();

    void validate();                         ///< b2 is triggered by needsValidation and sends inputValidated

    void highlightValidateState(bool isvalid); ///< b4 Is triggered by inputValidated

  Q_SIGNALS:
  // public:

    // void gotFocus(QWidgetInterface*);
    // void lostFocus();
    //
    // void valueChanged(QWidgetInterface*);    ///< a2 Is triggered by setValue of AVTI
    void needsValidation();                  ///< b1 Should be triggered if value needs validation. connect with validate
    void inputValidated(bool isvalid);       ///< b3 Is triggered by validate

  protected:

    virtual bool validateValue() { return true; }
    UserInteractionEventFilter *filter;

  private:

    QPalette wrongvalue;

    QWidget *widget;

};
