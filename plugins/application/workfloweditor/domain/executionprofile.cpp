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

#include <QString>
#include <QIcon>
#include "executionprofile.h"

namespace ExecutionProfiles {

  QString name(ExecutionProfile profile) {
    if (profile == ExecutionProfile::DEFAULT) return "Default";
    else if (profile == ExecutionProfile::SKIP) return "Skip";
    else if (profile == ExecutionProfile::DETACHED) return "Detached";
    else throw std::logic_error("Invalid execution profile");
  }

  ExecutionProfile fromString(const QString& string) {
    QString lowerString = string.toLower();
    if (lowerString == "default") return ExecutionProfile::DEFAULT;
    else if (lowerString == "skip") return ExecutionProfile::SKIP;
    else if (lowerString == "detached") return ExecutionProfile::DETACHED;
    else throw std::logic_error("Invalid execution profile");
  }

  QString iconPath(ExecutionProfile profile) {
    if (profile == ExecutionProfile::DEFAULT) {
      return ":/studio/plugins/application/workfloweditor/icons/fa-check-circle.svg";
    } else if (profile == ExecutionProfile::SKIP) {
      return ":/studio/plugins/application/workfloweditor/icons/fa-ban.svg";
    } else if (profile == ExecutionProfile::DETACHED) {
      return ":/studio/plugins/application/workfloweditor/icons/fa-background.svg";
    } else {
      throw std::logic_error("Invalid execution profile");
    }
  }

  QString help(ExecutionProfile profile) {
    if (profile == ExecutionProfile::DEFAULT) {
      return "Execute when reached by control flow";
    } else if (profile == ExecutionProfile::SKIP) {
      return "Node will be skipped";
    } else if (profile == ExecutionProfile::DETACHED) {
      return "Node will be executed in the background and execution status will be ignored";
    } else {
      throw std::logic_error("Invalid execution profile");
    }
  }
}
