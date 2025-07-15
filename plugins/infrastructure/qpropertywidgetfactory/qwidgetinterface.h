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

#include <QList>
#include <QWidget>

class QDialog;

#include "properties/data/valuetypeinterface/abstractvaluetypeinterface.h"
class EditDialogInterface;

class QWidgetInterface : public QWidget {
  Q_OBJECT

  public:

    virtual ~QWidgetInterface() = default;

    virtual const QWidget* getOuterWidget() const = 0;
    virtual QWidget* getOuterWidget() = 0;
    virtual QWidget* getWidget() const = 0;

    virtual AbstractValueTypeInterface* getValueTypeInterface() const = 0; // Kann das raus?

    virtual void addDialog(EditDialogInterface* dialog) = 0;
    virtual const QList<EditDialogInterface*>& getDialogList() = 0;

  // public Q_SLOTS:

    virtual void resetDefault() = 0;

    virtual void setFocus() = 0;
    virtual void clearFocus() = 0;
    virtual bool hasFocus() = 0;

  Q_SIGNALS: // TODO auf signals von lib/properties umstellen

    void gotFocus(QWidgetInterface*);
    void lostFocus();

    void valueChanged(QWidgetInterface*);    ///< a2 Is triggered by setValue of AVTI

};
