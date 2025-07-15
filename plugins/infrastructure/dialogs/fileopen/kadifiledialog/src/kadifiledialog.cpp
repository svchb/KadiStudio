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
#include <QProcess>
#include <QMessageBox>

#include <framework/pluginframework/pluginmanagerinterface.h>
#include <plugins/infrastructure/kadiintegration/kadiintegrationinterface.h>

#include "kadifiledialog.h"


KadiFileDialog::KadiFileDialog(LibFramework::PluginManagerInterface* pluginManager)
    : tmpfolder(QTemporaryDir(QDir::tempPath() + "/kadi_XXXXXX")) {
  kadiintegration = pluginManager->getInterface<KadiIntegrationInterface*>("/plugins/infrastructure/kadiintegration");

  downloadDialog = kadiintegration->createDownloadFromKadiDialog();

  if (not tmpfolder.isValid()) {
    qWarning() << tmpfolder.errorString();
    exit(-1);
  }
}

QString KadiFileDialog::getStorageMediumName() {
  return "Kadi";
}

void KadiFileDialog::setFileMode(FileOpenDialogInterface::FileMode mode) {
  filemode = mode;
}

void KadiFileDialog::selectFile(const QString& filepath) {
  downloadDialog->selectFile(filepath);
}

void KadiFileDialog::applyFilter(const QString& filter) {
  downloadDialog->applyFilter(filter);
}

bool KadiFileDialog::showFileOpenDialog() {
  bool rc = false;
  switch (filemode) {
    case AnyFile:
      rc = downloadDialog->showDialog(DLD_ANY_MODE);
      break;
    case ExistingFile:
      rc = downloadDialog->showDialog(DLD_FILE_MODE);
      break;
    case Directory:
      rc = downloadDialog->showDialog(DLD_IDENTIFIER_MODE);
      break;
    case ExistingFiles:
      // break;
    default :
      QMessageBox::critical(downloadDialog->getDialog(), "KadiFileDialog", QObject::tr("Not supported file mode %1").arg(filemode));
      rc = false;
  }
  applyFilter(""); // Hack but does the job
  return rc;
}

bool KadiFileDialog::validateAndLoadFilePath(const QString& kadiInstanceName, const QString& recordIdentifier, const QString& kadifilename) {
  // determine caching location
  QDir fileDir(getTmpFolder() + "/" + kadiInstanceName + "/" + recordIdentifier);

  // create folder structure
  fileDir.mkpath(fileDir.absolutePath());
  fileDir.cd(fileDir.absolutePath());

  downloadDialog->setFilename(kadiInstanceName, recordIdentifier, kadifilename);
  cachedfilepath = fileDir.absolutePath() + "/" + kadifilename;

  if (kadifilename.isEmpty()) return true; // is a record URI

  QProcess proc;
  proc.setWorkingDirectory(fileDir.absolutePath());
  proc.start("kadi-apy", {
    "records",
    "get-file",
    "-I", kadiInstanceName,
    "-R", recordIdentifier,
    "-n", kadifilename
  });
  proc.waitForFinished(5000);
  if (proc.exitCode() != 0) {
    return false;
  }

#if 0
  QNetworkRequest request;
  request.setUrl(QUrl(recordFileInfo.fileDownloadApiLink));
  request.setRawHeader("Authorization", (QString("Bearer ") + currentKadiInstance.token).toUtf8());

  preRequestHooks();
  auto requesthandler = new kadiutils::NetworkHandler(networkAccessManager, this);
  connect(requesthandler, &kadiutils::NetworkHandler::finished, this, [=](QNetworkReply* reply) {
    processFileReply(reply);
    postRequestHooks();
    delete requesthandler;
    QDialog::accept();
  });
  requesthandler->sendRequest(request);
#endif

  return true;
}

bool KadiFileDialog::validateAndLoadFilePath(const QString& filepath) {
  QString kadiInstanceName;
  QString recordIdentifier;
  QString kadifilename;

  if (not kadiintegration->getRecordIdentifier(filepath, kadiInstanceName, recordIdentifier, kadifilename) ||
      kadiInstanceName.isEmpty() || recordIdentifier.isEmpty()
     ) {
    qWarning() << "Malformed uri in " << filepath;
    return false;
  }

  return validateAndLoadFilePath(kadiInstanceName, recordIdentifier, kadifilename);
}

#if 0
void KadiFileDialog::processFileReply(QNetworkReply* reply) {
  int statusCode = kadiutils::getHttpCode(reply);

  if (statusCode > 0 && statusCode != 200) {
    return;
  }

  // save to kadi integration
  QString kadiInstanceName;
  QString recordIdentifier;
  QString kadifilename;
  if (not kadiutils::getRecordIdentifier(getFilePath(), kadiInstanceName, recordIdentifier, kadifilename)) {
    std::cout << "Broken filename " << getFilePath().toStdString() << " ." << std::endl;
    setCachedFileName(QString());
    return;
  }

  // determine caching location
  QDir fileDir(getTmpFolder() + "/" + kadiInstanceName + "/" + recordIdentifier);

  QString filePath = fileDir.absolutePath() + "/" + kadifilename;

  qDebug() << fileDir;
  qDebug() << filePath;

  // write file
  QFile cacheFile(filePath);
  if (cacheFile.open(QFile::WriteOnly)) {
    cacheFile.write(reply->readAll());
    cacheFile.flush();
    cacheFile.close();

    setCachedFileName(filePath);
  } else {
    qWarning() << "File '" << cacheFile.fileName() << "' could not be opened." << Qt::endl;

    setCachedFileName(QString());
  }
}
#endif

bool KadiFileDialog::openFilePath(const QString& filepath, QFile& qfile) {
  QString kadiInstanceName;
  QString recordIdentifier;
  QString kadifilename;

  if (not kadiintegration->getRecordIdentifier(filepath, kadiInstanceName, recordIdentifier, kadifilename) ||
      kadiInstanceName.isEmpty() || recordIdentifier.isEmpty() || kadifilename.isEmpty()
     ) {
    qWarning() << "Malformed uri in " << filepath;
    return false;
  }

  if (not validateAndLoadFilePath(kadiInstanceName, recordIdentifier, kadifilename)) {
    throw std::logic_error(QObject::tr("File does not exist").toStdString());
  }

  qfile.setFileName(getTmpFolder() + "/" + kadiInstanceName + "/" + recordIdentifier + "/" + kadifilename);
  if (!qfile.open(QIODevice::ReadOnly)) {
    throw std::logic_error(QObject::tr("File is not readable").toStdString());
  }
  return true;
}

bool KadiFileDialog::isCompatibleFilePath(const QString& filepath) {
  return filepath.startsWith("kadi://");
}

QString KadiFileDialog::getFilePath() {
  return downloadDialog->getLoadedFileName();
}

QString KadiFileDialog::getCachedFilePath() {
  return cachedfilepath;
}
