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

#include <QPushButton>
#include <QSpinBox>
#include <QLineEdit>
#include <QGridLayout>

#include <properties/data/valuetypeinterface/vectorvaluetype.h>

#include "../../editdialog.h"


/**
 * @brief
 * @ingroup    vector
 */
class EditDialog_VectorFill : public EditDialog {
  Q_OBJECT

  public:
    EditDialog_VectorFill();
    virtual ~EditDialog_VectorFill();

    QList<const std::type_info*> getUsableTypes() override {
      QList<const std::type_info*> list;
      list.append(&VectorValueType<std::string>::getValueTypeInfoStatic());
      return list;
    }

  private:
    QSpinBox      *vectorlength;
    QLineEdit     *prefix;

    QPushButton   *helpbutton;
    QPushButton   *okbutton;
    QPushButton   *cancelbutton;

    QGridLayout   *toplevellayout;

    QDialog* init(AbstractValueTypeInterface* avti) override;
    void deinit() override;

    void createUI();
    int setStartSize();

  private Q_SLOTS:

    void apply();
    void cancel();
    void help();

};
