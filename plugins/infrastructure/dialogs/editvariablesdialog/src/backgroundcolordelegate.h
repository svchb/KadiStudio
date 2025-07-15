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

#include <QStyledItemDelegate>
#include <QStyleOptionViewItem>

#include "editvariablesdialog.h"

/**
 *  @brief      A QStyledItemDelegate used to show rows of the
 *              table in different colors.
 *  @ingroup    src
 *
 * See https://stackoverflow.com/a/10220126/3997725
 */
class BackgroundColorDelegate : public QStyledItemDelegate {

  public:
    BackgroundColorDelegate(EditVariablesDialog* dialog)
      : QStyledItemDelegate(dialog), dialog(dialog) {
    }

    QColor calculateColorForRow(int row) const {
      return dialog->getColorForRow(row);
    }

    void initStyleOption(QStyleOptionViewItem* option, const QModelIndex& index) const {
      QStyledItemDelegate::initStyleOption(option, index);

      QStyleOptionViewItem *style_option =
        qstyleoption_cast<QStyleOptionViewItem*>(option);

      style_option->backgroundBrush = QBrush(calculateColorForRow(index.row()));
    }
  private:
    EditVariablesDialog *dialog;
};
