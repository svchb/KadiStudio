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

#include <QToolTip>
#include <QTextEdit>
#include <QRegularExpression>

#include "../../qwidgetinterface.h"
#include "qvtitooltip.h"


void QVTIToolTip::showText(const QPoint &pos, const QWidgetInterface* qwti) {
  if (qwti) {
    const auto hint = qwti->getValueTypeInterface()->getHint();
    QString tooltiptext;
    if (hint->hasEntry("label")) {
      tooltiptext += "<h3>" + QString::fromStdString(hint->getEntry("label")) + "</h3>";
    }
    if (hint->getDescription().size() > 0) {
      // make html from markdown
      QString description = hint->getDescription().c_str();
      QTextEdit textedit;
      textedit.setMarkdown(description);
      tooltiptext.append(textedit.toHtml());

    }

    if (hint->hasEntry("unit")) {
      QString unitstring = QString::fromStdString(hint->getEntry("unit"));

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

      tooltiptext += "<br/><br/>Physical unit: ";
      auto terms = unitstring.split("/");
      terms.front().replace(QRegularExpression("\\(?([^\\)]*)\\)?"),"\\1");
      if (terms.size() != 2 || terms.back().isEmpty()) {
        tooltiptext += "[" + unitstring + "]";
      } else {
        terms.back().replace(QRegularExpression("\\({0,1}([^\\)]*)\\) {0,1}"),"\\1");
        tooltiptext += "<big>[<sup>" + terms.front() + "</sup><big>/</big><sub>" + terms.back() + "</sub>]</big>";
      }
    }
    QToolTip::showText(pos, tooltiptext);
  }
}

bool QVTIToolTip::isVisible() {
  return QToolTip::isVisible();
}
