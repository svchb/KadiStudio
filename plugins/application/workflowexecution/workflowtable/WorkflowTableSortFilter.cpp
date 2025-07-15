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

#include <QtCore/QDateTime>

#include "WorkflowTableSortFilter.h"

WorkflowTableSortFilter::WorkflowTableSortFilter(QObject *parent) : QSortFilterProxyModel(parent) {

}

bool WorkflowTableSortFilter::lessThan(const QModelIndex &left, const QModelIndex &right) const {
  QVariant leftData = sourceModel()->data(left);
  QVariant rightData = sourceModel()->data(right);

  if (leftData.type() == QVariant::DateTime) {
    return leftData.toDateTime() < rightData.toDateTime();
  } else if (left.column() == 2) {
    return leftData.toInt() < rightData.toInt();
  } else {
    return QString::localeAwareCompare(leftData.toString(), rightData.toString()) < 0;
  }
}
