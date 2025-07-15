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

#include <QEventLoop>
#include <QHBoxLayout>
#include <QMenu>
#include <QMessageBox>
#include <QPushButton>
#include <QTimer>
#include <QToolButton>
#include <QSpacerItem>

#include <pluginframework/pluginmanagerinterface.h>
#include <plugins/infrastructure/dialogs/fileopen/fileopendialoginterface.h>
#include <properties/data/properties/derivative/openfileproperty.h>

#include "qvtiwidget_string_directory.h"

QPropertyFileOpen::QPropertyFileOpen(LibFramework::PluginManagerInterface* pluginmanager, Property* property, QWidget* parent)
    : QPropertyWidget(property, new QWidget(parent)),
      lineedit(new QLineEdit(getWidget())),
      toolButton(new QToolButton(getWidget())),
      validateButton(new QPushButton(getWidget())),
      lastUsedDialogInterface(nullptr),
      revalidateFilePath(false),
      validationCommandConnected(false) {

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
    connect(action, &QAction::triggered, getWidget(), [=, this] {
      std::string allowedextensions = property->getHint()->getEntry("allowedextensions");
      if (!allowedextensions.empty()) {
        dialogInterface->applyFilter(QString::fromStdString(allowedextensions));
      } else {
        dialogInterface->applyFilter(tr("All files or folders (*)"));
      }
      if (dialogInterface->showFileOpenDialog()) {
        lastUsedDialogInterface = dialogInterface;
        revalidateFilePath = false; // because the text is about to be set, don't revalidate via the connect (once)
        validateButton->hide();
        lineedit->setText(dialogInterface->getCachedFilePath()); // triggers setValue(...) via the connect
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
}

void QPropertyFileOpen::synchronizeVTI() {
  std::string value;
  VTIWidget::getValue<std::string>(value);
  lineedit->setText(QString::fromStdString(value));
}

void QPropertyFileOpen::setValue(const QString& value) {
  VTIWidget::setValue<std::string>(value.toStdString());
  if (revalidateFilePath) {
    if (lastUsedDialogInterface == nullptr) { // when the user directly types something into the line edit
      return;
    }

    validateButton->show();

    if (validationCommandConnected) {
      disconnect(validateButton, &QPushButton::clicked, nullptr, nullptr);
      validationCommandConnected = false;
    }

    connect(validateButton, &QPushButton::clicked, getWidget(), [=, this] {
      validateButton->setText(tr("Revalidating..."));
      // the five lines below allow to process the setText event in the line
      // above, so that the text on the button is actually visible while the
      // file path is revalidated
      QEventLoop loop;
      QTimer t;
      QTimer::connect(&t, &QTimer::timeout, &loop, &QEventLoop::quit);
      t.start(10); // 10 milliseconds
      loop.exec();

      if (lastUsedDialogInterface->validateAndLoadFilePath(value)) {
        validateButton->setText(tr("Revalidate"));
        validateButton->hide();
      } else {
        validateButton->setText(tr("Revalidation failed"));
        QTimer::singleShot(3000, validateButton, [=, this] { validateButton->setText(tr("Revalidate")); });
      }
    });
    validationCommandConnected = true;

  } else {
    revalidateFilePath = true; // skip revalidation only once
  }
}

void QPropertyFileOpen::checkLength() {
  if (lineedit->text().size() >= lineedit->maxLength()) {
    QMessageBox::warning(getWidget(), tr("File open"), tr("text too long for line edit, so it was chopped"));
  }
}
