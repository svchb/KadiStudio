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

#include <QString>
class QByteArray;

/**
 * @brief      Show the dialog in which the user is able to upload a
 *             file to Kadi.
 * @ingroup    kadiintegration
 */
class UploadToKadiDialogInterface {

public:
  virtual ~UploadToKadiDialogInterface() = default;

  virtual bool showDialog(const QByteArray &fileContent, const QString& enforce_file_extension, const QString& filename = "", const QString& identifier = "", const QString& instance = "") = 0;

  virtual const QString& getFileName() const = 0;

  virtual const QString& getRecordIdentifier() const = 0;

};
