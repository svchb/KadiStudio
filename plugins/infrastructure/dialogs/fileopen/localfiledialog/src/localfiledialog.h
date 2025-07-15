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

#include <QFileDialog>

#include "../../fileopendialoginterface.h"


namespace LibFramework {
  class PluginManagerInterface;
}

class QString;


/** @class Provides a dialog in which the user is able to select a file from the local file system
 *  @ingroup    localfiledialog
 */
class LocalFileDialog : public QFileDialog, public FileOpenDialogInterface {
  Q_OBJECT

public:

  explicit LocalFileDialog(LibFramework::PluginManagerInterface* pluginManager);

  ~LocalFileDialog() override = default;

  QString getStorageMediumName() override;

  void setFileMode(FileOpenDialogInterface::FileMode mode) override;

  void selectFile(const QString& filepath) override;

  void applyFilter(const QString& filter) override;

  bool showFileOpenDialog() override;

  bool validateAndLoadFilePath(const QString& filepath) override;

  bool openFilePath(const QString& filepath, QFile& qfile) override;

  bool isCompatibleFilePath(const QString& filepath) override;

  QString getFilePath() override;

  QString getCachedFilePath() override;

private:

  QString localFilePath;

};
