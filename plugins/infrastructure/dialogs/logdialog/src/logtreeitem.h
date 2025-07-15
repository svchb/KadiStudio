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

#include <QTreeWidgetItem>

/**
 * @brief      A customized tree item for the log tree
 * @ingroup    src
 */
class LogTreeItem : public QTreeWidgetItem {

  public:
    explicit LogTreeItem(const QString& name, const QString& id);

    QString getId() const;
    void setId(const QString& id);
    void setName(const QString& name);
    void setState(const QString& state);
    void setOrder(int order);

  private:
    bool operator<(const QTreeWidgetItem& other) const {
      const int column = 1;
      return text(column).toInt() < other.text(column).toInt();
    }
    void updateTooltip();
    QString name;
    QString id;
    int order;
    bool loading_animation;
};
