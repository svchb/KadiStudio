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

#include "tooldescription.h"


void ToolDescription::setVersion(const QString& version) {
#if 1
  QRegularExpression rx = QRegularExpression("[0-3][0-9]\\.[0-1][0-9]\\.[0-9]{4}");
  QRegularExpressionMatch match = rx.match(version);
  if (match.hasMatch()) {
    QStringList releasedate = match.captured(0).split(".");
    int year = releasedate.at(2).toInt();
    int month = releasedate.at(1).toInt();
    int day = releasedate.at(0).toInt();
    toolreleasedate.setDate(year, month, day);
  }
  rx = QRegularExpression("([0-9]+\\.?)+");
  match = rx.match(version);
  if (match.hasMatch()) {
    toolversion = match.captured(0);
  }
#else
  toolversion = version;
#endif
}

void ToolDescription::removeHelp() {
  int pcount = toolparams.size();

  for (int i = 0; i < pcount; i++) {
    if (toolparams[i].getLongName() == "help" || toolparams[i].getLongName() == "xmlhelp") {
      toolparams.erase(toolparams.begin() + i);
      i--;
      pcount--;
    }
  }
}
