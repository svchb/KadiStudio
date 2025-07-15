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

#include <QDialog>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QGridLayout>
#include <QList>
#include <QSpinBox>
#include <QScrollBar>
#include <QKeySequence>
#include <QAction>
#include <QWheelEvent>
#include <QComboBox>
#include <QProgressDialog>
#include <QProgressBar>
#include <QMessageBox>

class QWidgetInterface;

#include <properties/data/valuetypeinterface/matrixvaluetype.h>

#include "../../editdialog.h"


/**
 * @brief
 * @ingroup    matrix
 */
class InfileWidget_MatrixEditEventFilter : public QObject {
  Q_OBJECT

  public:

    InfileWidget_MatrixEditEventFilter(QObject* parent);

  Q_SIGNALS:

    void triggeredEnter();
    void altUp();
    void altDown();
    void altLeft();
    void altRight();
    void apply();
    void shortjumpi();
    void shortjumpj();
    void editn();
    void editm();
    void override();
    void left();
    void right();
    void up();
    void down();

  protected:

    virtual bool eventFilter(QObject* obj, QEvent* event);
};

class EditDialog_Matrix : public EditDialog {
  Q_OBJECT

  public:

    EditDialog_Matrix();
    virtual ~EditDialog_Matrix();

    QList<const std::type_info*> getUsableTypes() override {
      QList<const std::type_info*> list;
      list.append(&AbstractMatrixValueType::getValueTypeInfoStatic());
      return list;
    }

  private:

    int                                 visiblelineeditx;
    int                                 visiblelineedity;
    int                                 visiblelineeditstartx;
    int                                 visiblelineeditstarty;
    int                                 matrixsizex;
    int                                 matrixsizey;
    int                                 mostleftedit;
    int                                 mosttopedit;
    int                                 oldmatrixsizex;
    int                                 oldmatrixsizey;

    bool                                editableM;
    bool                                editableN;
    bool                                convertable;

    int                                 position[2];
    int                                 focusposition[2]; // the wiget pos which has focus

    InfileWidget_MatrixEditEventFilter *filter;

    bool                                validate;

    QFont                               normal;
    QFont                               bold;
    QPalette                            pal;
    QPalette                            lightPal;

    QList<QLabel*>                      labellistx;
    QList<QLabel*>                      labellisty;
    QList<QList<QVTIWidget*> >          linelist;
    QList<QList<QString> >              matrix;

    QDialog* init(AbstractValueTypeInterface* avti) override;
    void deinit() override;
    void fillTitel();
    void fillScroll();
    void fillEdit();
    void fillJump();
    void fillControl();
    bool matrixise(const QString& parentstring);
    void updateLine();
    void createEdit();
    void clearEdit();
    void setStartSize(const QString& parentstring);
    void save();
    void wheelEvent(QWheelEvent* event) override;
    void searchFocus();
    void jump(int targetx, int targety);
    void clearHighlighting();
    void setHighlight();

    QVBoxLayout   *toplevellayout;

    QHBoxLayout   *vscrolllayout;
    QGridLayout   *editlayout;
    QHBoxLayout   *jumpboxlayout;
    QHBoxLayout   *controlslayout;
    QHBoxLayout   *hscrolllayout;
    QHBoxLayout   *midlayout;

    QPushButton   *lengthbutton;
    QSpinBox      *matrixlengthx;
    QSpinBox      *matrixlengthy;
    QPushButton   *jumpbutton;
    QSpinBox      *jumpmarkx;
    QSpinBox      *jumpmarky;
    QPushButton   *convertbutton;
    QComboBox     *convertdrop;
    QVTIWidget    *convertbox;

    // jumpline Labels
    QLabel      *ilabel;
    QLabel      *jlabel;
    QLabel      *mlabel;
    QLabel      *nlabel;
    QSpacerItem *spacer;

    QScrollBar  *hscrollbar;
    QScrollBar  *vscrollbar;

  private Q_SLOTS:

    void apply();
    void reset();
    void cancel();
    void jumpTriggered();
    void resize();
    void moveX();
    void moveY();
    void convert();
    void moveSliderLeft();
    void moveSliderRight();
    void moveSliderDown();
    void moveSliderUp();
    void enterPressed();
    void focusGained(QWidgetInterface* qwti);
    void stepDown();
    void stepUp();
    void stepLeft();
    void stepRight();
    void help();
    void checkValidate(bool validate);

};
