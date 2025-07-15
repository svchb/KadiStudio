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

#include "../../fileopendialoginterface.h"

#include <QString>
#include <QTemporaryDir>


namespace LibFramework {
  class PluginManagerInterface;
}

class DownloadFromKadiDialogInterface;
class KadiIntegrationInterface;

/**
 * @class Provides a dialog in which the user is able to select a file from Kadi
 *  @ingroup    kadifiledialog
 */
class KadiFileDialog : public FileOpenDialogInterface {

public:
  explicit KadiFileDialog(LibFramework::PluginManagerInterface* pluginManager);

  ~KadiFileDialog() override = default;

  QString getStorageMediumName() override;

  void setFileMode(FileOpenDialogInterface::FileMode mode) override;

  void selectFile(const QString& filepath) override;

  void applyFilter(const QString& filter) override;

  bool showFileOpenDialog() override;

  bool validateAndLoadFilePath(const QString& filepath) override;

  bool openFilePath(const QString& filepath, QFile& qfile) override;

  bool isCompatibleFilePath(const QString& filepath) override;

  QString getFilePath() override;
  /**
   * @note if the user selected a workflow file then this method is not idempotent because the filter is cleared when
   * returning the file path.
   */
  QString getCachedFilePath() override;

private:

  bool validateAndLoadFilePath(const QString& kadiInstanceName, const QString& recordIdentifier, const QString& kadifilename);

  QString getTmpFolder() {
    return tmpfolder.path();
  }

  KadiIntegrationInterface *kadiintegration;

  FileOpenDialogInterface::FileMode filemode;
  DownloadFromKadiDialogInterface *downloadDialog;

  QString cachedfilepath;

  QTemporaryDir tmpfolder;

};
