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

#include <QRegularExpression>

#include <framework/pluginframework/pluginmanagerinterface.h>

#include "localfiledialog.h"


LocalFileDialog::LocalFileDialog([[maybe_unused]]LibFramework::PluginManagerInterface *pluginManager)
    : QFileDialog(),
      localFilePath("") {
  // this->setOption(QFileDialog::DontUseNativeDialog, true);
}

QString LocalFileDialog::getStorageMediumName() {
  return "Local File System";
}

void LocalFileDialog::setFileMode(FileOpenDialogInterface::FileMode mode) {
  switch (mode) {
    case FileOpenDialogInterface::AnyFile:
      QFileDialog::setFileMode(QFileDialog::AnyFile);
      break;
    case FileOpenDialogInterface::ExistingFile:
      QFileDialog::setFileMode(QFileDialog::ExistingFile);
      break;
    case FileOpenDialogInterface::Directory:
      QFileDialog::setFileMode(QFileDialog::Directory);
      QFileDialog::setOption(QFileDialog::ShowDirsOnly);
      break;
    case FileOpenDialogInterface::ExistingFiles:
      QFileDialog::setFileMode(QFileDialog::ExistingFiles);
      break;
  }
}

void LocalFileDialog::selectFile(const QString& filepath) {
  QFileDialog::selectFile(filepath);
}

void LocalFileDialog::applyFilter(const QString& filter) {
  this->setNameFilter(filter);
}

bool LocalFileDialog::showFileOpenDialog() {
  bool rc = false;
  if (this->exec()) {
    localFilePath = this->selectedFiles().first();
    rc = true;
  }
  applyFilter(""); // Hack but does the job
  return rc;
}

bool LocalFileDialog::validateAndLoadFilePath(const QString& filepath) {
  if (isCompatibleFilePath(filepath)) {
    localFilePath = filepath;
    this->selectFile(filepath);
    return true;
  } else {
    return false;
  }
}

bool LocalFileDialog::openFilePath(const QString& filepath, QFile& qfile) {
  if (!QFileInfo::exists(filepath)) {
    throw std::logic_error(tr("File does not exist").toStdString());
  }
  qfile.setFileName(filepath);
  if (!qfile.open(QIODevice::ReadOnly)) {
    throw std::logic_error(tr("File is not readable").toStdString());
  }
  return true;
}

bool LocalFileDialog::isCompatibleFilePath(const QString& filepath) {
  QString filepathresolver = filepath;
  filepathresolver.replace(QRegularExpression("^~"), QDir::homePath());

  return QFileInfo::exists(filepathresolver);
}

QString LocalFileDialog::getFilePath() {
  return localFilePath;
}

QString LocalFileDialog::getCachedFilePath() {
  return localFilePath;
}
