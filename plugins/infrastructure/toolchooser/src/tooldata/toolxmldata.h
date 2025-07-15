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

#include "toolcache.h"
#include "tooldescription.h"


/**
 * @brief      Class for parsing XML help
 *
 * Parses a tool identification string or an executable / toolname to
 * fill ToolDescription with tool data.
 * @ingroup    tooldialog
 */
class ToolXMLData {

  public:
    ToolXMLData();
    virtual ~ToolXMLData();

    bool createFromXML(const QString& xmlstring, const QString& command);
    bool createXML(const QString &toolidentificationstring);
    bool createToolDescription(const QString& command);

    const ToolDescription& Description() const;

    void resetCache();

  private:
    ToolCache       toolcache;
    ToolDescription tooldescription;

};
