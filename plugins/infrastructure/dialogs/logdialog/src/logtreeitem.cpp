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

#include <QLabel>
#include <QMovie>

#include "logtreeitem.h"

LogTreeItem::LogTreeItem(const QString& name, const QString& id)
    : QTreeWidgetItem({name, ""}, UserType), loading_animation(false) {
  setName(name);
  setId(id);
}

QString LogTreeItem::getId() const {
  return id;
}

void LogTreeItem::setId(const QString& id) {
  this->id = id;
  updateTooltip();
}

void LogTreeItem::updateTooltip() {
  QString tooltip;

  if (!name.isEmpty()) {
    tooltip.append(QString("Log Item: %1").arg(name));
  }
  if (!name.isEmpty() && !id.isEmpty() && name != id) {
    tooltip.append("\n");
  }
  if (!id.isEmpty() && name != id) {
    tooltip.append(QString("Id: %1").arg(id));
  }
  setToolTip(0, tooltip);
}

void LogTreeItem::setName(const QString& name) {
  this->name = name;
  if (loading_animation) {
    setText(0, "      " + name); // spaces are placeholder for the loading animation
  } else {
    setText(0, name);
  }
  updateTooltip();
}

void LogTreeItem::setState(const QString& state) {
  loading_animation = false;
  if (state == "EX") {
    setIcon(0, QIcon(":/studio/plugins/infrastructure/dialogs/logdialog/icons/fa-check-circle.svg"));
    setForeground(0, QColor("black"));
  } else if (state == "ER") {
    setIcon(0, QIcon(":/studio/plugins/infrastructure/dialogs/logdialog/icons/fa-circle-xmark.svg"));
    setForeground(0, QColor("darkred"));
  } else if (state == "TBS") {
    setIcon(0, QIcon(":/studio/plugins/infrastructure/dialogs/logdialog/icons/fa-ban.svg"));
    setForeground(0, QColor("gray"));
  } else if (state == "R") {
    setIcon(0, QIcon());
    setForeground(0, QColor("black"));
    if (!treeWidget()->itemWidget(this, 0)) {
      auto icon_label = new QLabel();
      icon_label->setAutoFillBackground(false);
      icon_label->setFixedSize(18, 18);
      icon_label->setContentsMargins(5, 0, 0, 0);
      auto movie = new QMovie(":/studio/plugins/infrastructure/dialogs/logdialog/icons/loading.gif");
      icon_label->setMovie(movie);
      treeWidget()->setItemWidget(this, 0, icon_label);
      movie->setScaledSize(QSize(icon_label->sizeHint().width(), 5));
      movie->start();
    }
    loading_animation = true;
  } else if (state == "NI") {
    setIcon(0, QIcon(":/studio/plugins/infrastructure/dialogs/logdialog/icons/fa-circle-question.svg"));
    setForeground(0, QColor("orange"));
  } else if (state == "TBE") {
    setIcon(0, QIcon(":/studio/plugins/infrastructure/dialogs/logdialog/icons/fa-circle-pause.svg"));
    setForeground(0, QColor("black"));
  } else {
    setIcon(0, QIcon());
  }

  setName(name);
  if (!loading_animation) {
    // remove loading animation again in case it was active for this item's previous state
    if (auto label = dynamic_cast<QLabel*>(treeWidget()->itemWidget(this, 0))) {
      delete label->movie(); // widget does not take ownership of the movie
    }
    treeWidget()->removeItemWidget(this, 0);
  }
}

void LogTreeItem::setOrder(int order) {
  LogTreeItem::order = order;
  setText(1, QString::number(order));
}
