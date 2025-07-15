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
#include <QRegularExpression>

#include "qlineedit_withunitlabel.h"

QLineEdit_withUnitLabel::QLineEdit_withUnitLabel(QString unitstring, QWidget* parent) : QLineEdit(parent) {
  if (not unitstring.isEmpty()) {
    unitlabel = new QLabel(this);
    unitlabel->setStyleSheet("QLabel { color : gray; word-spacing : 0px;}");

    int fontheight = fontMetrics().height()*11/10;

    QMap<QString, QString> replacemap = {
      {"^0", "⁰"}, {"^1", "¹"},  {"^2", "²"},  {"^3", "³"},  {"^4", "⁴"},  {"^5", "⁵"},  {"^6", "⁶"},  {"^7", "⁷"},  {"^8", "⁸"},  {"^9", "⁹"},  {"*", "·"}
    };

    int i = 0;
    QRegularExpressionMatch match;
    while ((match = QRegularExpression("(\\^[0-9]|\\*)").match(unitstring, i)).hasMatch()) {
      QString replacewith = replacemap[match.capturedTexts().front()];
      unitstring.replace(match.capturedStart(), match.capturedTexts().front().size(), replacewith);
      i = match.capturedStart() + replacewith.size();
    }

    auto terms = unitstring.split("/");
    terms.front().replace(QRegularExpression("\\(?([^\\)]*)\\)?"),"\\1");
    if (terms.size() != 2 || terms.back().isEmpty()) {
      unitlabel->setText(unitstring);
    } else {
      terms.back().replace(QRegularExpression("\\({0,1}([^\\)]*)\\){0,1}"),"\\1");
      unitlabel->setText("<div style='font-size: " + QString::number(fontheight) + "px;'><sup>" + terms.front() + "</sup>/<sub>" + terms.back() + "</sub></div>");
    }

    connect(this, &QLineEdit::textChanged, this, [this](const QString& /*value*/) {
      updateUnitPos();
    });
  } else {
    unitlabel = nullptr;
  }
}

void QLineEdit_withUnitLabel::showEvent(QShowEvent * event) {
  if (unitlabel) updateUnitPos();
  QLineEdit::showEvent(event);
}

void QLineEdit_withUnitLabel::updateUnitPos() {
  int unitlabelwidth = fontMetrics().horizontalAdvance(text()) + 3*fontMetrics().averageCharWidth()/2;
  unitlabel->move(rect().left() + textMargins().left() + unitlabelwidth, rect().center().y() - unitlabel->height() / 2);
}
