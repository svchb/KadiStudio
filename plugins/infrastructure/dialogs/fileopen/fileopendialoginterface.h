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

#include <framework/pluginframework/pluginmanagerinterface.h>
#include <framework/pluginframework/pluginclientinterface.h>

class QString;
class QFile;

/**
 * @class      An interface to provide uniform access to file dialogs
 *             across multiple providers (e.g.: Local FS, Kadi, ...)
 * @ingroup    dialogs
 */
class FileOpenDialogInterface : public LibFramework::PluginClientInterface {

public:
  typedef enum {
    AnyFile       = 0,
    ExistingFile  = 1,
    Directory     = 2,
    ExistingFiles = 3,
  } FileMode;


  ~FileOpenDialogInterface() override = default;

  /**
   * @brief Get the name of the storage medium.
   *
   * This name will be displayed in the menu of the tool button in the file open property in the form
   * of "From <storage_medium_name>".
   *
   * @return the name or description of the storage medium on which the dialog operates.
   */
  virtual QString getStorageMediumName() = 0;

  /**
   * @brief Selects the file mode for the dialog.
   *
   * @sa FileMode
   */
  virtual void setFileMode(FileMode mode) = 0;

  virtual void selectFile(const QString& filepath) = 0;

  /**
   * @brief Permits to filter the displayed files in the dialog.
   *
   * Some local standard file dialogs provide means to filter by file extension. For this to work the filter must be
   * specified in the following format:
   *
   * \<Name of the filter\> (*.\<fileext1\> *.\<fileext2\> ...)
   *   - Example 1: "Pace3D (*.p3s *.p3v *.p3b *.p3simgeo)"
   *   - Example 2: "Images (*.png *.jpg *.jpeg *.gif)"
   *
   * @param filter Filter to apply
   */
  virtual void applyFilter(const QString& filter) = 0;

  /**
   * @brief Displays the dialog and enables the user to pick a file to open or load.
   * @return true if a file was selected, false else
   */
  virtual bool showFileOpenDialog() = 0;

  /**
   * @brief Allows the user to input a file path which in turn will be loaded from the storage medium.
   * @param filepath Path to a file on the storage medium
   * @return true if the path was valid and selected, false else
   */
  virtual bool validateAndLoadFilePath(const QString& filepath) = 0;

  /**
   * @brief Open the file from the storage medium.
   * @param filepath Path to a file on the storage medium
   * @param qfile file handle to the file
   * @return true if the path was valid and selected, false else
   */
  virtual bool openFilePath(const QString& filepath, QFile& qfile) = 0;

  /**
   * @brief Checks if the provided file path is compatible with this storage medium.
   * @param filepath potential Path to a file on the storage medium
   * @return true if the path is compatible with this storage medium
   */
  virtual bool isCompatibleFilePath(const QString& filepath) = 0;

  /**
   * @brief Getter to retrieve the path to the URI of the file.
   * @return the path to the selected file
   */
  virtual QString getFilePath() = 0;

  /**
   * @brief Getter to retrieve the path to the opened or loaded file on the local filesystem.
   * @return the path to the selected file
   */
  virtual QString getCachedFilePath() = 0;

  static FileOpenDialogInterface* getCompatibleFileOpenPlugin(LibFramework::PluginManagerInterface* pluginmanager, const QString& filepath) {
    LibFramework::Interfaces fileopendialoginterfaces = pluginmanager->getInterfaces("/plugins/infrastructure/dialogs/fileopen/");
    for (auto fileopendialoginterface : fileopendialoginterfaces) {

      const LibFramework::InterfaceContainer *const interfacecontainer = fileopendialoginterface.second;
      if (interfacecontainer) {
        auto fileopendialoginterface = interfacecontainer->getInterface<FileOpenDialogInterface*>();
        if (fileopendialoginterface->isCompatibleFilePath(filepath)) {
          return fileopendialoginterface;
        }
      }

    }
    return nullptr;
  }

};
