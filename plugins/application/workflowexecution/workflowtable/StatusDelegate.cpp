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

#include <QApplication>
#include <QPainter>

#include "StatusDelegate.h"

StatusDelegate::StatusDelegate(QObject *parent) : QStyledItemDelegate(parent) {

}

void StatusDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const {
  int progress = index.data(Qt::DisplayRole).toInt();
  if (progress < 100) {
    QStyleOptionProgressBar progressBarOption;
    progressBarOption.state = QStyle::State_Enabled;
    progressBarOption.direction = QApplication::layoutDirection();
    progressBarOption.rect = option.rect;
    progressBarOption.fontMetrics = QFontMetrics(option.font);
    progressBarOption.minimum = 0;
    progressBarOption.maximum = 100;
    progressBarOption.textAlignment = Qt::AlignCenter;
    progressBarOption.textVisible = true;

    progressBarOption.progress = progress < 0 ? 0 : progress;
    progressBarOption.text = QString("%1%").arg(progressBarOption.progress);

    QApplication::style()->drawControl(QStyle::CE_ProgressBar, &progressBarOption, painter);
  }
}
