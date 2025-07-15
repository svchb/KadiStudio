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

#include <vector>
#include <memory>

#include "logline.h"

class LogDialog;

/**
 * @brief      A class representing log content for a given context. It can contain any number of lines,
 *             which can a) store a real string or b) a reference to a line, and it can have a parent.
 *             Whenever a line is added, a reference to this line will also be added to the parent to incorporate a
 *             hierarchical log. The LogDialog is notified when a new line was added to allow updating the GUI.
 * @ingroup    src
 */
class LogContent {
  public:
    LogContent(const std::string& context, LogDialog* dialog);
    LogLine* add(const std::string& string);
    LogLine* add(LogLine& ref);
    void clear();
    std::string toString() const;
    void setParent(LogContent* parent);
    std::string getContextForLine(size_t line_number) const;
    const std::string& getContext() const;

  private:
    std::vector<std::unique_ptr<LogLine>> lines;
    LogContent *parent{};
    std::string context;
    LogDialog *dialog;
};
