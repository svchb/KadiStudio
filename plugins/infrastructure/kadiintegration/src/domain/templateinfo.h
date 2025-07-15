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

#include "recordinfo.h"


/**
 * @brief      Holds information about a kadi template.
 * @ingroup    kadiintegration
 */
struct TemplateInfo {
  long id;
  QString identifier;
  QString type; ///< record or extras
  QString title;
  QString description;
  QString metadata; ///< holds a json string
  QString extrametadata; ///< holds a json string; is the same as metadata if the template is of type "extras"
  RecordInfo recordInfo; ///< if the template is of type record this will be set

  bool operator==(const TemplateInfo& other) const;
};
