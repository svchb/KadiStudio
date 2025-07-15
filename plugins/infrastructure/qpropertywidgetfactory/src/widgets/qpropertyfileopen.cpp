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

#include <QLoggingCategory>
#include <QEventLoop>
#include <QWidget>
#include <QHBoxLayout>
#include <QPushButton>
#include <QToolButton>
#include <QSpacerItem>
#include <QTimer>
#include <QMenu>
#include <QAction>
#include <QMessageBox>

#include <framework/enhanced/qlineeditclearable.h>
#include <framework/pluginframework/pluginmanagerinterface.h>

#include <plugins/infrastructure/dialogs/fileopen/fileopendialoginterface.h>

#include "qpropertyfileopen.h"
#include "properties/data/valuetypeinterfacehint.h"

QPropertyFileOpen::QPropertyFileOpen(LibFramework::PluginManagerInterface* pluginmanager, Property* property, QWidget* parent)
    : QPropertyWidget(property, new QWidget(parent)),
      lineedit(new QLineEditClearable(getWidget())),
      validateButton(new QPushButton(getWidget())),
      toolButton(new QToolButton(getWidget())) {

  this->pluginmanager = pluginmanager;

  lineedit->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
  getWidget()->setLayout(new QHBoxLayout());

  locationMenu = new QMenu(getWidget());

  // load all plugins implementing the file open dialog interface
  auto allDialogInterfaces = pluginmanager->getInterfaces("/plugins/infrastructure/dialogs/fileopen");
  for (const auto &i : allDialogInterfaces) {
    auto *dialogInterface = i.second->getInterface<FileOpenDialogInterface *>();
    if (dialogInterface == nullptr) {
      continue;
    }

    auto *action = new QAction("From " + dialogInterface->getStorageMediumName(), this);
    connect(action, &QAction::triggered, getWidget(), [this, dialogInterface] {
      const ValueTypeInterfaceHint *hint = getProperty()->getHint();

      FileOpenDialogInterface::FileMode mode = FileOpenDialogInterface::AnyFile;
      if (hint->hasEntry("filemode")) {
        std::string filemodestring = hint->getEntry("filemode");
        if (filemodestring == "AnyFile") {
          mode = FileOpenDialogInterface::AnyFile;
        } else if (filemodestring == "ExistingFile") {
          mode = FileOpenDialogInterface::ExistingFile;
        } else if (filemodestring == "Directory") {
          mode = FileOpenDialogInterface::Directory;
        } else if (filemodestring == "ExistingFiles") {
          mode = FileOpenDialogInterface::ExistingFiles;
        } else {
          qWarning() << "Unknown file mode; falling back to AnyFile";
        }
      }
      dialogInterface->setFileMode(mode);

      dialogInterface->selectFile(QString::fromStdString(getValue<std::string>()));

      std::string allowedextensions = hint->getEntry("allowedextensions");
      if (!allowedextensions.empty()) {
        dialogInterface->applyFilter(QString::fromStdString(allowedextensions));
      }
      if (dialogInterface->showFileOpenDialog()) {
        lineedit->setText(dialogInterface->getFilePath()); // triggers setValue(...) via the connect
        validateButton->hide();
      }
    });

    locationMenu->addAction(action);
  }

  validateButton->setText(tr("Revalidate"));
  validateButton->hide();

  toolButton->setMenu(locationMenu);
  toolButton->setText(tr("Open File"));
  toolButton->setToolTip(tr("open file dialog for the specific storage medium provider"));
  toolButton->setToolButtonStyle(Qt::ToolButtonTextOnly);
  toolButton->setPopupMode(QToolButton::InstantPopup);

  getWidget()->layout()->setContentsMargins(0, 0, 0, 0); // disable spacing, otherwise the widget doesn't align with others in QPropertyPanel with Grid layout type
  getWidget()->layout()->setSpacing(0);

  getWidget()->layout()->addWidget(lineedit);
  getWidget()->layout()->addItem(new QSpacerItem(10, 1));
  getWidget()->layout()->addWidget(validateButton);
  getWidget()->layout()->addWidget(toolButton);

  connect(this->lineedit, SIGNAL(textEdited(QString)), this, SLOT(checkLength()));

  connect(lineedit, &QLineEdit::textChanged, this, &QPropertyFileOpen::setValue);

  connect(validateButton, &QPushButton::clicked, this, [this] {
    validateButton->setText(tr("Revalidating..."));
    // the five lines below allow to process the setText event in the line
    // above, so that the text on the button is actually visible while the
    // file path is revalidated
    QEventLoop loop;
    QTimer t;
    QTimer::connect(&t, &QTimer::timeout, &loop, &QEventLoop::quit);
    t.start(10); // 10 milliseconds
    loop.exec();

    QString filepath = QString::fromStdString(VTIWidget::getValue<std::string>());

    FileOpenDialogInterface *matchingFileDialogInterface = FileOpenDialogInterface::getCompatibleFileOpenPlugin(this->pluginmanager, filepath);

    if (matchingFileDialogInterface && matchingFileDialogInterface->validateAndLoadFilePath(filepath)) {
      validateButton->setText(tr("Revalidate"));
      validateButton->hide();
    } else {
      validateButton->setText(tr("Revalidation failed"));
      QTimer::singleShot(3000, validateButton, [this] { validateButton->setText(tr("Revalidate")); });
    }
  });

}

void QPropertyFileOpen::synchronizeVTI() {
  const std::string &actualvalue = getValue<std::string>();
  int cursor_position = lineedit->cursorPosition();
  lineedit->setText(QString::fromStdString(actualvalue));
  lineedit->setCursorPosition(cursor_position);
}

void QPropertyFileOpen::setValue(const QString& value) {
  const std::string &actualvalue = getValue<std::string>();
  std::string newvalue = value.toStdString();
  if (actualvalue == newvalue) return;
  VTIWidget::setValue(newvalue);

  if (newvalue.empty()) {
    validateButton->hide();
  } else {
    validateButton->show();
  }
}

void QPropertyFileOpen::checkLength() {
  if (lineedit->text().size() >= lineedit->maxLength()) {
    QMessageBox::warning(getWidget(), tr("File open"), tr("text too long for line edit, so it was chopped"));
  }
}
