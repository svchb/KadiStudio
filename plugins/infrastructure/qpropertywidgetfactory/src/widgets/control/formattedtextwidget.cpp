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
#include <properties/data/ambassador.h>

#include "formattedtextwidget.h"

FormattedTextWidget::FormattedTextWidget(Property* property, QWidget* parent)
    : QPropertyWidget(property, new QLabel(parent)) {
}

void FormattedTextWidget::synchronizeVTI() {
  auto ambassador = getInnerAmbassador();

  std::string text, format;
  ambassador->getValue("text", text);
  ambassador->getValue("format/selected", format);
  auto label = reinterpret_cast<QLabel *>(getWidget());
  if (format == "markdown") {
    label->setTextFormat(Qt::MarkdownText);
  } else if (format == "html") {
    label->setTextFormat(Qt::RichText); // note: supports only a subset of html
  } else {
    label->setTextFormat(Qt::PlainText);
  }
  label->setText(QString::fromStdString(text));
}
