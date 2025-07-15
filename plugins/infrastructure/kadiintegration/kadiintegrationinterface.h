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

#include <QByteArray>
#include <QString>

#include <framework/pluginframework/pluginclientinterface.h>

#include "downloadfromkadidialoginterface.h"
#include "uploadtokadidialoginterface.h"


/**
 * @ingroup    kadiintegration
 * @brief      Provides means to integrate Kadi by providing config
 *             editing and uploading and downloading of workflows.
 */
class KadiIntegrationInterface : public LibFramework::PluginClientInterface {

public:
  ~KadiIntegrationInterface() override = default;

  // Still needed to load library, move stuff at right place and remove this class/plugin

  virtual DownloadFromKadiDialogInterface* createDownloadFromKadiDialog() = 0;
  virtual UploadToKadiDialogInterface* createUploadToKadiDialog() = 0;

  virtual bool getRecordIdentifier(const QString &filepath, QString& kadiInstanceName, QString& recordIdentifier, QString& kadifilename) = 0;

};
