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
class QLabel;
class QHBoxLayout;
class QVBoxLayout;
class QGridLayout;
class QSpinBox;
class QGridLayout;
class QScrollBar;
class QSpacerItem;

class QWidgetInterface;

#include <properties/data/valuetypeinterface/vectorvaluetype.h>

#include "../../editdialog.h"


/**
 * @brief
 * @ingroup    vector
 */
class InfileWidget_VectorEditEventFilter : public QObject {
  Q_OBJECT

  public:
    InfileWidget_VectorEditEventFilter(QObject* parent);

  Q_SIGNALS:
    void triggeredEnter();
    void apply();
    void altLeft();
    void altRight();
    void shortjump();
    void edit();
    void left();
    void right();
    void override();

  protected:
    virtual bool eventFilter(QObject *obj, QEvent *event);
};

class EditDialog_Vector : public EditDialog {
  Q_OBJECT

  public:
    EditDialog_Vector();
    virtual ~EditDialog_Vector();

    QList<const std::type_info*> getUsableTypes() override {
      QList<const std::type_info*> list;
      list.append(&AbstractVectorValueType::getValueTypeInfoStatic());
      return list;
    }

  private:

    int visiblelineedit;
    int visiblelineeditstart;
    int vectorsize;
    int mostleftedit;
    int oldvectorsize;
    int position;
    int focusposition;

    bool editable;
    bool convertable;

    InfileWidget_VectorEditEventFilter* filter;

    QFont normal;
    QFont bold;
    QPalette pal;
    QPalette lightPal;

    QList<QLabel*>          labellist;
    QList<QVTIWidget*> linelist;
    QList<QString>          vector;

    void fillTitel();
    void fillEdit();
    void fillJump();
    void fillControl();
    void vectorise(const QString& parentstring);
    void updateLine();
    void createEdit();
    void clearEdit();
    void save();
    void setHighlight();
    void clearHighlighting();
    void searchFocus();
    void jump(int index);

    QDialog* init(AbstractValueTypeInterface* avti) override;
    void deinit() override;

    int setStartSize(const QString& parentstring);

    QVBoxLayout *toplevellayout;

    QGridLayout *editlayout;
    QHBoxLayout *jumpboxlayout;
    QHBoxLayout *controlslayout;
    QHBoxLayout *scrolllayout;

    QPushButton   *lengthbutton;
    QSpinBox      *vectorlength;
    QPushButton   *jumpbutton;
    QSpinBox      *jumpmark;
    QPushButton   *convertbutton;
    QVTIWidget    *convertbox;

    QScrollBar    *hscrollbar;
    QSpacerItem   *spacer;

  private Q_SLOTS:

    void apply();
    void reset();
    void cancel();
    void jumpTriggered();
    void stepLeft();
    void stepRight();
    void resize();
    void move();
    void convert();
    void enterPressed();
    void focusGained(QWidgetInterface* qwti);
    void help();
    void checkValidate(bool validate);

};
