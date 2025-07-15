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

#include <QStyle>
#include <QFileDialog>
#include <QHBoxLayout>
#include <QFileSystemModel>
#include <QCompleter>

#include "searchdirectorywidget.h"
#include "plugins/infrastructure/dialogs/registertooldialog/src/registertooldialog.h"

class FileSystemModel : public QFileSystemModel {
  public:
    FileSystemModel(QObject* parent = nullptr)
      : QFileSystemModel(parent) {
    }

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override {
      if (role == Qt::DisplayRole && index.column() == 0) {
        QString path = QDir::toNativeSeparators(filePath(index));
        if (path.endsWith(QDir::separator())) {
          path.chop(1);
        }
        return path;
        // return path.split(QDir::separator()).last();
      }

      return QFileSystemModel::data(index, role);
    }
};


SearchDirectoryWidget::SearchDirectoryWidget(QWidget *parent)
    : QWidget(parent) {
  QHBoxLayout *widgetlayout = new QHBoxLayout;

  QCompleter *completer = new QCompleter(this);
  completer->setMaxVisibleItems(/*maxVisibleSpinBox->value()*/10);

  directorylineedit = new QLineEdit;

  // Unsorted QFileSystemModel
  // QFileSystemModel *fsModel = new QFileSystemModel(completer);
  // FileSystemModel that shows full paths
  fsModel = new FileSystemModel(completer);
  fsModel->setFilter(QDir::Dirs | QDir::NoDot | QDir::NoDotDot);
  fsModel->setRootPath(QString());
  fsModel->setOption(QFileSystemModel::DontWatchForChanges);
  completer->setModel(fsModel);
  completer->setWrapAround(false);

  connect(completer, qOverload<const QString&>(&QCompleter::highlighted), this, [completer,this]([[maybe_unused]]const QString& text) {
    completer->complete();
    directorylineedit->setFocus();
  });
  connect(directorylineedit, &QLineEdit::textEdited, this, [this]([[maybe_unused]]const QString& text) {
    fsModel->setRootPath(text);
  });

  directorylineedit->setCompleter(completer);
  directorylineedit->setPlaceholderText(tr("Enter search path"));
  directorylineedit->setClearButtonEnabled(true);

  minusbutton = new QPushButton("-");
  minusbutton->setToolTip(tr("Remove path"));
  minusbutton->setFixedWidth(directorylineedit->sizeHint().height());
  minusbutton->setFixedHeight(directorylineedit->sizeHint().height());
  QSizePolicy sp_retain = minusbutton->sizePolicy();
  sp_retain.setRetainSizeWhenHidden(true);
  minusbutton->setSizePolicy(sp_retain);
  widgetlayout->addWidget(minusbutton);

  // directorylineedit->setReadOnly(true);
  directorylineedit->setMinimumWidth(700);
  widgetlayout->addWidget(directorylineedit);

  QPushButton *directorybutton = new QPushButton();
  directorybutton->setIcon(this->style()->standardIcon(QStyle::SP_DirOpenIcon));
  directorybutton->setToolTip(tr("Browse path"));
  directorybutton->setFixedWidth(directorylineedit->sizeHint().height());
  directorybutton->setFixedHeight(directorylineedit->sizeHint().height());
  directorybutton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
  connect(directorybutton, SIGNAL(clicked()), this, SLOT(selectDirectory()));
  widgetlayout->addWidget(directorybutton);

  plusbutton = new QPushButton("+");
  plusbutton->setToolTip(tr("Add path"));
  plusbutton->setFixedWidth(directorylineedit->sizeHint().height());
  plusbutton->setFixedHeight(directorylineedit->sizeHint().height());
  widgetlayout->addWidget(plusbutton);

  setLayout(widgetlayout);

  directorylineedit->setFocus();
}

SearchDirectoryWidget::~SearchDirectoryWidget() {
}

QPushButton *SearchDirectoryWidget::plusButton() {
  return plusbutton;
}

QPushButton *SearchDirectoryWidget::minusButton() {
  return minusbutton;
}

QLineEdit *SearchDirectoryWidget::lineEdit() {
  return directorylineedit;
}

void SearchDirectoryWidget::selectDirectory() {
  QDir currentdir(directorylineedit->text());
  QString current = QDir::homePath();

  if (currentdir.exists()) {
    current = directorylineedit->text();
  }

  QString dir = QFileDialog::getExistingDirectory(this, tr("Select Directory"),
                current, QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

  if (!dir.isEmpty()) {
    directorylineedit->setText(dir);
  }
}
