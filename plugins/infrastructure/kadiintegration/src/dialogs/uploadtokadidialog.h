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

#pragma once

#include <QDialog>
#include "../../uploadtokadidialoginterface.h"

// forward declarations
namespace LibFramework {
  class PluginManagerInterface;
}

class KadiConfigInterface;

class QString;
class QCheckBox;
class QComboBox;
class QDialogButtonBox;
class QLineEdit;
class QRadioButton;


/**
 * @brief      Show the dialog in which the user is able to upload a
 *             file to Kadi.
 * @ingroup    kadiintegration
 */
class UploadToKadiDialog : public QDialog, public UploadToKadiDialogInterface {
Q_OBJECT

public:
  explicit UploadToKadiDialog(LibFramework::PluginManagerInterface* pluginmanager, QWidget* parent = nullptr);

  bool showDialog(const QByteArray &fileContent, const QString& enforce_file_extension, const QString& filename = "", const QString& identifier = "", const QString& instance = "");

  const QString& getFileName() const override {
    return filename;
  }

  const QString& getRecordIdentifier() const override {
    return record_identifier;
  }

private:
  void showKadiConfigDialog();

  void loadIdentifiersFromDownloadDialog();

  void loadFileNameFromDownloadDialog();

  void updateKadiInstanceSelectioBox(bool selectDefaultInstance);

  void accept() override;

  bool tryToCreateRecord(const QString& identifier);

  bool uploadFile(const QByteArray& fileContent, const QString& filename, const QString& identifier, bool use_fallback_method = false);

  KadiConfigInterface *kadiConfigInterface;

  QByteArray loadedFileContent;
  QString filename;
  QString record_identifier;
  QString enforce_file_extension;

  QComboBox *kadiInstanceSelectionBox;
  QLineEdit *filenameField;
  QComboBox *identifierField;
  QRadioButton *radioCreateNewRecord;
  QRadioButton *radioAddToExistingRecord;
  QCheckBox *overwriteFiles;

  QDialogButtonBox *buttonBox;

  LibFramework::PluginManagerInterface *pluginmanager;

};
