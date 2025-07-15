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

#include "inserttooldialog.h"

#include <QDialogButtonBox>
#include <QPushButton>
#include <QVBoxLayout>
#include <QMap>
#include <QVariant>
#include <plugins/infrastructure/toolchooser/toolchooserinterface.h>

InsertToolDialog::InsertToolDialog(LibFramework::PluginManagerInterface* pluginmanager, QWidget *parent)
    : QDialog(parent) {

  setModal(true);
  setLayout(new QVBoxLayout());

  toolchooser_interface = pluginmanager->getInterface<ToolChooserInterface*>("/plugins/infrastructure/toolchooser")->clone();

  layout()->addWidget(toolchooser_interface->getWidget());

  QDialogButtonBox *buttonbox = new QDialogButtonBox();
  buttonbox->addButton(QDialogButtonBox::Ok);
  connect(buttonbox, &QDialogButtonBox::accepted, this, &QDialog::accept);
  buttonbox->addButton(QDialogButtonBox::Cancel);
  connect(buttonbox, &QDialogButtonBox::rejected, this, &QDialog::reject);
  layout()->addWidget(buttonbox);

  buttonbox->button(QDialogButtonBox::Ok)->setEnabled(false);

  toolchooser_interface->setToolSelectedCallback([buttonbox]() {
    buttonbox->button(QDialogButtonBox::Ok)->setEnabled(true);
  });
  toolchooser_interface->setToolResetCallback([buttonbox]() {
    buttonbox->button(QDialogButtonBox::Ok)->setEnabled(false);
  });
}

InsertToolDialog::~InsertToolDialog() {
  delete toolchooser_interface;
}

QMap<QString, QVariant> InsertToolDialog::getSelectedToolInformation() {
  if (exec() == Accepted) {
    return toolchooser_interface->getToolData();
  }
  return {};
}
