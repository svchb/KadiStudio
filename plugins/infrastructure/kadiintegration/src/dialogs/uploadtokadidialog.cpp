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

#include <QString>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QCheckBox>
#include <QComboBox>
#include <QPushButton>
#include <QRadioButton>
#include <QDialogButtonBox>
#include <QMessageBox>
#include <QProcess>
#include <QToolTip>
#include <QTemporaryFile>
#include <QTemporaryDir>
#include <QTextStream>

#include <framework/enhanced/qlineeditclearable.h>

#include <framework/pluginframework/pluginmanagerinterface.h>
#include <plugins/infrastructure/kadiconfig/kadiconfiginterface.h>
#include "../kadiintegration.h"
#include "../utils/kadiutils.h"

#include "downloadfromkadidialog.h"
#include "uploadtokadidialog.h"


UploadToKadiDialog::UploadToKadiDialog(LibFramework::PluginManagerInterface* pluginmanager, QWidget* parent) : QDialog(parent), loadedFileContent(), pluginmanager(pluginmanager) {
  setWindowTitle(tr("Upload File To Kadi"));

  kadiConfigInterface = pluginmanager->getInterface<KadiConfigInterface *>("/plugins/infrastructure/kadiconfig");

  QVBoxLayout *wholeLayout = new QVBoxLayout(this);
  QFormLayout *dialogLayout = new QFormLayout();

  auto *hbox1 = new QHBoxLayout();
  kadiInstanceSelectionBox = new QComboBox(this);
  hbox1->addWidget(kadiInstanceSelectionBox, 10);

  QPushButton *kadiConfigBtn = new QPushButton("");
  kadiConfigBtn->setIcon(this->style()->standardIcon(QStyle::SP_FileDialogDetailedView));
  kadiConfigBtn->setToolTip(tr("Open Kadi configuration"));
  kadiConfigBtn->setFixedHeight(kadiInstanceSelectionBox->sizeHint().height());
  kadiConfigBtn->setFixedWidth(kadiInstanceSelectionBox->sizeHint().height());
  connect(kadiConfigBtn, &QPushButton::clicked, this, &UploadToKadiDialog::showKadiConfigDialog);
  hbox1->addWidget(kadiConfigBtn, 1);
  dialogLayout->addRow(tr("Kadi Instance:"), hbox1);

  auto *hbox2 = new QHBoxLayout();

  identifierField = new QComboBox();
  identifierField->setMinimumWidth(250);
  identifierField->setEditable(true);
  identifierField->setDuplicatesEnabled(false);
  hbox2->addWidget(identifierField, 1);

  QPushButton *fetchIdentifiers = new QPushButton("");
  fetchIdentifiers->setIcon(QIcon(":/studio/plugins/infrastructure/kadiintegration/img/fa-cloud-arrow-down.svg"));
  fetchIdentifiers->setToolTip(tr("Fetch record identifiers via dialog"));
  fetchIdentifiers->setFixedHeight(identifierField->sizeHint().height());
  fetchIdentifiers->setFixedWidth(identifierField->sizeHint().height());
  connect(fetchIdentifiers, &QPushButton::clicked, this, &UploadToKadiDialog::loadIdentifiersFromDownloadDialog);
  hbox2->addWidget(fetchIdentifiers);

  auto *hbox3 = new QHBoxLayout();

  filenameField = new QLineEditClearable();
  filenameField->setMinimumWidth(450);
  hbox3->addWidget(filenameField, 1);

  QPushButton *fetchFilenameNameBtn = new QPushButton();
  fetchFilenameNameBtn->setIcon(QIcon(":/studio/plugins/infrastructure/kadiintegration/img/fa-cloud-arrow-down.svg"));
  fetchFilenameNameBtn->setToolTip(tr("Fetch filename via dialog"));
  fetchFilenameNameBtn->setFixedHeight(filenameField->sizeHint().height());
  fetchFilenameNameBtn->setFixedWidth(filenameField->sizeHint().height());
  connect(fetchFilenameNameBtn, &QPushButton::clicked, this, &UploadToKadiDialog::loadFileNameFromDownloadDialog);
  hbox3->addWidget(fetchFilenameNameBtn);

  dialogLayout->addRow(tr("Record Identifier:"), hbox2);
  dialogLayout->addRow(tr("Filename:"), hbox3);

  wholeLayout->addLayout(dialogLayout);
  wholeLayout->addStretch(10);

  auto *hbox4 = new QGridLayout();
  radioCreateNewRecord = new QRadioButton(tr("Create new record"));
  radioAddToExistingRecord = new QRadioButton(tr("Add file to existing record"));

  hbox4->addItem(new QSpacerItem(1,1,QSizePolicy::Expanding,QSizePolicy::Expanding),0,0);
  hbox4->addWidget(radioCreateNewRecord, 0, 1);
  hbox4->addWidget(radioAddToExistingRecord, 0, 2);

  overwriteFiles = new QCheckBox(tr("Overwrite existing file"));
  connect(radioCreateNewRecord, &QRadioButton::clicked, overwriteFiles, &QCheckBox::setDisabled);
  connect(radioAddToExistingRecord, &QRadioButton::clicked, overwriteFiles, &QCheckBox::setEnabled);

  //connect(radioCreateNewRecord, &QRadioButton::clicked, fetchIdentifiers, &QPushButton::setDisabled);
  //connect(radioAddToExistingRecord, &QRadioButton::clicked, fetchIdentifiers, &QPushButton::setEnabled);
  hbox4->addWidget(overwriteFiles, 1, 2);

  wholeLayout->addLayout(hbox4);
  wholeLayout->addStretch(10);

  buttonBox = new QDialogButtonBox();
  buttonBox->addButton(tr("Upload"), QDialogButtonBox::AcceptRole);
  buttonBox->addButton(QDialogButtonBox::Cancel);
  connect(buttonBox, &QDialogButtonBox::accepted, this, &UploadToKadiDialog::accept);
  connect(buttonBox, &QDialogButtonBox::rejected, this, &UploadToKadiDialog::reject);
  wholeLayout->addWidget(buttonBox);

  updateKadiInstanceSelectioBox(true);
}

bool UploadToKadiDialog::showDialog(const QByteArray& fileContent, const QString& enforce_file_extension, const QString& filename, const QString& identifier, const QString& instance) {
  loadedFileContent = fileContent;
  this->enforce_file_extension = enforce_file_extension;

  identifierField->clear();

  if (!filename.isEmpty() && !identifier.isEmpty() && !instance.isEmpty()) {
    kadiInstanceSelectionBox->setCurrentText(instance);
    filenameField->setText(filename);
    identifierField->setCurrentText(identifier);
    radioAddToExistingRecord->click();
  } else {
    filenameField->clear();
    identifierField->setCurrentText("");
    radioCreateNewRecord->click();
  }

  exec();
  return result() == UploadToKadiDialog::Accepted;
}

void UploadToKadiDialog::showKadiConfigDialog() {
  kadiConfigInterface->showDialog(); // blocks until dialog is closed
  updateKadiInstanceSelectioBox(false);
}

void UploadToKadiDialog::loadIdentifiersFromDownloadDialog() {
  DownloadFromKadiDialog downloadDialog(pluginmanager);
  bool loaded = downloadDialog.showDialog(DLD_IDENTIFIER_MODE);

  auto savedCurrentText = identifierField->currentText();

  if (loaded) {
    identifierField->clear();
    for (const auto& recordInfo : downloadDialog.getAllRecords()) {
      identifierField->addItem(recordInfo.identifier);
    }

    QString kadiInstanceName;
    QString recordIdentifier;
    QString kadifilename;

    kadiutils::getRecordIdentifier(downloadDialog.getLoadedFileName(), kadiInstanceName, recordIdentifier, kadifilename);

    kadiInstanceSelectionBox->setCurrentText(kadiInstanceName);
    identifierField->setCurrentText(recordIdentifier);
  } else {
    identifierField->setCurrentText(savedCurrentText);
  }
}

void UploadToKadiDialog::loadFileNameFromDownloadDialog() {
  DownloadFromKadiDialog downloadDialog(pluginmanager);
  downloadDialog.setFilename(kadiInstanceSelectionBox->currentText(), "", "");
  if (downloadDialog.showDialog(DLD_FILE_MODE)) {
    QString kadiInstanceName;
    QString recordIdentifier;
    QString kadifilename;

    kadiutils::getRecordIdentifier(downloadDialog.getLoadedFileName(), kadiInstanceName, recordIdentifier, kadifilename);
    kadiInstanceSelectionBox->setCurrentText(kadiInstanceName);
    identifierField->addItem(recordIdentifier);
    identifierField->setCurrentText(recordIdentifier);
    filenameField->setText(kadifilename);
  }
}

void UploadToKadiDialog::updateKadiInstanceSelectioBox(bool selectDefaultInstance) {
  QString lastInstance = kadiInstanceSelectionBox->currentText();
  bool lastUsedInstanceExists = false;

  kadiInstanceSelectionBox->clear();

  int defaultIndex = -1;
  for (const auto &kadiInstance : kadiConfigInterface->getAllInstances()) {
    kadiInstanceSelectionBox->addItem(kadiInstance.name);
    if (kadiInstance.isDefaultInstance) {
      defaultIndex = kadiInstanceSelectionBox->count() - 1;
    }
    if (kadiInstance.name == lastInstance) {
      lastUsedInstanceExists = true;
    }
  }

  if (selectDefaultInstance && defaultIndex != -1) {
    kadiInstanceSelectionBox->setCurrentIndex(defaultIndex);
  } else if (lastUsedInstanceExists) {
    kadiInstanceSelectionBox->setCurrentText(lastInstance);
  }
}

void UploadToKadiDialog::accept() {
  QString filename = filenameField->text();
  if (!enforce_file_extension.isEmpty() && !filename.endsWith(enforce_file_extension) && !filename.isEmpty()) {
    filename.append(enforce_file_extension);
  }

  QString recordIdentifier = identifierField->currentText();

  if (filename.isEmpty()) {
    QToolTip::showText(this->pos() + buttonBox->pos(), tr("The filename must not be empty"), buttonBox);
    return;
  }
  if (recordIdentifier.isEmpty()) {
    QToolTip::showText(this->pos() + buttonBox->pos(), tr("The record identifier must not be empty"), buttonBox);
    return;
  }

  if (radioCreateNewRecord->isChecked()) {
    if (!tryToCreateRecord(recordIdentifier)) {
      return;  // failed (tryToCreateRecord() shows a message box with the error)
    }
  }

  if (uploadFile(loadedFileContent, filename, recordIdentifier)) {
    this->filename = filename;
    this->record_identifier = recordIdentifier;
    QDialog::accept(); // success
  }
}

bool UploadToKadiDialog::tryToCreateRecord(const QString& identifier) {
  QProcess proc;

  // try to create the record
  proc.start("kadi-apy", {
    "records",
    "create",
    "-I", kadiInstanceSelectionBox->currentText(),
    "-i", identifier,
    "-e"  // return error if the record is not created
  });
  proc.waitForFinished();

  if (proc.exitCode() != 0) {
    kadiutils::showWarningWithProcOutput(this, tr("File Upload"), tr("Record creation failed. Output from kadi-apy:"), proc);
    return false;
  }

  return true;
}

bool UploadToKadiDialog::uploadFile(const QByteArray& fileContent, const QString& fileName, const QString& identifier, bool use_fallback_method) {

  QTemporaryDir tmpdir;
  QTemporaryFile tmpfile;
  if (!use_fallback_method) {
    if (!tmpdir.isValid()) {
      int result = QMessageBox::question(this, tr("File Upload"),
                           tr("Uploading failed: No temporary directory available. Do you want to reattempt using direct upload?"), QMessageBox::Ok | QMessageBox::Cancel);
      if (result == QMessageBox::Ok) {
        use_fallback_method = true;
      } else {
        return false;
      }
    } else if (!tmpfile.open()) {
      int result = QMessageBox::question(this, tr("File Upload"),
                           tr("Uploading failed: No temporary file could be created. Do you want to reattempt using direct upload?"), QMessageBox::Ok | QMessageBox::Cancel);
      if (result == QMessageBox::Ok) {
        use_fallback_method = true;
      } else {
        return false;
      }
    }
  }

  QStringList arguments;
  if (use_fallback_method) {
    arguments = QStringList {
      "records",
      "add-string-as-file",
      "-I", kadiInstanceSelectionBox->currentText(),
      "-R", identifier,
      "-s", fileContent,
      "-n", fileName
    };
  } else {
    QTextStream stream(&tmpfile);
    stream << fileContent << Qt::endl;
    tmpfile.rename(tmpdir.path() + "/" + fileName); // this is important because the basename is used as filename in kadi

    arguments = QStringList {
      "records",
      "add-files",
      "-I", kadiInstanceSelectionBox->currentText(),
      "-R", identifier,
      "-n", tmpfile.fileName()
    };
  }

  if (radioAddToExistingRecord->isChecked() && overwriteFiles->isChecked()) {
    arguments.append("-f");
  }

  QProcess proc;
  proc.start("kadi-apy", arguments);
  proc.waitForFinished();

  QString stdoutAsStr(proc.readAllStandardOutput());
  QRegularExpression fileAlreadyExistsPattern("A file with the name '.*' already exists.");

  if (proc.exitCode() != 0) {
    if (stdoutAsStr.contains(fileAlreadyExistsPattern)) {
      int result = QMessageBox::question(this, tr("File Upload"), tr("Uploading the file failed, because it already exists in this record, do you want to overwrite it?"), QMessageBox::Ok | QMessageBox::Cancel);
      if (result == QMessageBox::Ok) {
        this->overwriteFiles->click();
        this->radioAddToExistingRecord->click();
        return uploadFile(fileContent, fileName, identifier, use_fallback_method);
      }
      return false;
    } else {
      kadiutils::showWarningWithProcOutput(this, tr("File Upload"), tr("Uploading the file failed. Output from kadi-apy:"), proc);
      return false;
    }
  } else {
    QMessageBox::information(this, tr("File Upload"), tr("Upload successful."));
    return true;
  }
}
