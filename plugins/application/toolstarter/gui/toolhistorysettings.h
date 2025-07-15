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

#include <QStringList>

class QSettings;
class QDateTime;

/**
 * @brief      Saves and loads parameter configuration of recently used tools.
 * @ingroup    toolstarter
 */
class ToolHistorySettings {

  public:
    ToolHistorySettings();
    ~ToolHistorySettings();

    void save(const QString &toolidentificationstring, const QDateTime &datetime);
    QStringList load();


  private:
    int maxentrycount;

    QSettings *settings;

};
