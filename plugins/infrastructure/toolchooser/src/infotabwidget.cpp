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

#include <QEvent>
#include <QLayout>
#include <QLabel>
#include <QTextEdit>
#include <QSpacerItem>

#include "tooldata/tooldescription.h"
#include "infotabwidget.h"


InfoTabWidget::InfoTabWidget(QWidget *parent)
    : QTabWidget(parent) {
  toolnamelabel = new QLabel;
  toolpathlabel = new QLabel;
  toolversionlabel = new QLabel;
  toolreleasedatelabel = new QLabel;

  tooldescriptiontextedit = new QTextEdit;
  tooldescriptiontextedit->setReadOnly(true);
  tooldescriptiontextedit->setFocusPolicy(Qt::ClickFocus);
  tooldescriptiontextedit->setTextInteractionFlags(Qt::TextSelectableByMouse);

  QTextEditEventFilter *tooldescriptiontextediteventfilter = new QTextEditEventFilter(this);
  tooldescriptiontextedit->installEventFilter(tooldescriptiontextediteventfilter);
  connect(tooldescriptiontextediteventfilter, &QTextEditEventFilter::lostFocus, [this]() {
    QTextCursor cursor = tooldescriptiontextedit->textCursor();
    cursor.clearSelection();
    tooldescriptiontextedit->setTextCursor(cursor);
  });

  toolexampleusagetextedit = new QTextEdit;
  toolexampleusagetextedit->setReadOnly(true);
  toolexampleusagetextedit->setFocusPolicy(Qt::ClickFocus);
  toolexampleusagetextedit->setTextInteractionFlags(Qt::TextSelectableByMouse);

  QTextEditEventFilter *toolexampleusagetextediteventfilter = new QTextEditEventFilter(this);
  toolexampleusagetextedit->installEventFilter(toolexampleusagetextediteventfilter);
  connect(toolexampleusagetextediteventfilter, &QTextEditEventFilter::lostFocus, [this]() {
    QTextCursor cursor = toolexampleusagetextedit->textCursor();
    cursor.clearSelection();
    toolexampleusagetextedit->setTextCursor(cursor);
  });

  QGridLayout *gridlayout = new QGridLayout;
  QLabel *name = new QLabel(tr("Name:"));
  QLabel *path = new QLabel(tr("Path:"));
  QLabel *version = new QLabel(tr("Version:"));
  QLabel *releaseDate = new QLabel(tr("Release date:"));

  gridlayout->addWidget(name, 0, 0);
  gridlayout->addWidget(path, 1, 0);
  gridlayout->addWidget(version, 2, 0);
  gridlayout->addWidget(releaseDate, 3, 0);
  gridlayout->setAlignment(name, Qt::AlignRight);
  gridlayout->setAlignment(path, Qt::AlignRight);
  gridlayout->setAlignment(version, Qt::AlignRight);
  gridlayout->setAlignment(releaseDate, Qt::AlignRight);
  gridlayout->addWidget(toolnamelabel, 0, 1);
  gridlayout->addWidget(toolpathlabel, 1, 1);
  gridlayout->addWidget(toolversionlabel, 2, 1);
  gridlayout->addWidget(toolreleasedatelabel, 3, 1);
  gridlayout->addItem(new QSpacerItem(1, 0, QSizePolicy::Expanding), 0, 4);

  QWidget *toolinfowidget = new QWidget;
  toolinfowidget->setLayout(gridlayout);

  this->addTab(toolinfowidget, tr("Info"));
  this->addTab(tooldescriptiontextedit, tr("Description"));
  this->addTab(toolexampleusagetextedit, tr("Example usage"));
  this->setCurrentIndex(1);
  this->setMinimumHeight(110);
}

InfoTabWidget::~InfoTabWidget() {
}

void InfoTabWidget::showHelpText(const QString& description, const QString& example, bool flag) {
  QPalette p;
  p.setColor(QPalette::Text, (flag) ? Qt::gray : Qt::black);

  tooldescriptiontextedit->setPalette(p);
  tooldescriptiontextedit->setText(description);
  toolexampleusagetextedit->setPalette(p);
  toolexampleusagetextedit->setText(example);
}

void InfoTabWidget::clear() {
  showHelpText(tr("Select a tool from above to show its description."),
               tr("Select a tool from above to show its example usage."), true);
  toolnamelabel->clear();
  toolpathlabel->clear();
  toolversionlabel->clear();
  toolreleasedatelabel->clear();
}

void InfoTabWidget::setToolInformation(const ToolDescription* tooldescription) {
  if (tooldescription == nullptr || tooldescription->name().isEmpty()) {
    clear();
  } else {
    QString name = tooldescription->shortName();
    toolnamelabel->setText(name);
    QString path = tooldescription->getToolpath();
    toolpathlabel->setText(path);
    QString version = tooldescription->version();
    toolversionlabel->setText(version);
    QDate releasedate = tooldescription->releaseDate();
    toolreleasedatelabel->setText(releasedate.toString(Qt::ISODate));

    QString description = tooldescription->description();
    QString exampletext = tooldescription->example();
    showHelpText(description, exampletext, false);
  }
}
