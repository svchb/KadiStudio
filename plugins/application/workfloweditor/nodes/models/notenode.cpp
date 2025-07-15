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

#include "notenode.h"

NoteNode::NoteNode()
    : WorkflowNode(), textfield(new QPlainTextEdit()) {
  textfield->setAttribute(Qt::WA_NoSystemBackground);
  textfield->setAttribute(Qt::WA_DontShowOnScreen);
  //textfield->setFixedWidth(640);
  textfield->viewport()->setAutoFillBackground(false);
  textfield->setStyleSheet("border: 0px");
  textfield->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  textfield->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

  QFontMetrics metrics(textfield->document()->defaultFont());
  fontheight = metrics.height();
  textfield->setFixedHeight(fontheight/3 + (FONTHEIGHT_EXT + fontheight)); // size when empty

  setNodeStyle(EditorConfig::NOTE_STYLE);

  //connect(textfield->document()->documentLayout(), &QAbstractTextDocumentLayout::documentSizeChanged, this, &NoteNode::updateHeight);
  connect(textfield, &QPlainTextEdit::textChanged, this, &NoteNode::updateHeight);
  syncLines();
}

QJsonObject NoteNode::save() const {
  QJsonObject modeljson = WorkflowNode::save();
  modeljson["text"] = textfield->document()->toPlainText();
  // modeljson["width"] = textfield->document()->size().width();
  // modeljson["height"] = textfield->document()->size().height();
  return modeljson;
}

void NoteNode::load(QJsonObject const &p) {
  QJsonValue textjson = p["text"];

  if (!textjson.isUndefined()) {
    textfield->document()->setPlainText(textjson.toString());
  }
  syncLines();
  updateHeight();
}

void NoteNode::updateHeight() {
  double size = textfield->document()->documentLayout()->documentSize().height();
  textfield->setFixedHeight(fontheight/3 + (int)((FONTHEIGHT_EXT + fontheight) * size));
  emit NodeDelegateModel::embeddedWidgetSizeUpdated();
}
/**
  * This slot allows to trigger line wrapping explicitly to get the correct number of lines in updateHeight()
  * afterwards.
  */
void NoteNode::syncLines() {
  textfield->setLineWrapMode(QPlainTextEdit::NoWrap);
  textfield->setLineWrapMode(QPlainTextEdit::WidgetWidth);
}
