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

#include <sstream>
#include "logdialog.h"
#include "logcontent.h"

LogContent::LogContent(const std::string& context, LogDialog* dialog)
    : context(context), dialog(dialog) {
}

LogLine* LogContent::add(const std::string& string) {
  auto line = std::make_unique<LogLineString>(*this, string);
  LogLine *result = line.get();
  if (parent) {
    parent->add(*line);
  }
  lines.push_back(std::move(line));
  dialog->logContentUpdated(context, *result);
  return result;
}

LogLine* LogContent::add(LogLine& ref) {
  auto new_ref = std::make_unique<LogLineRef>(ref);
  LogLine *result = new_ref.get();
  lines.push_back(std::move(new_ref));
  if (parent) {
    parent->add(ref);
  }
  dialog->logContentUpdated(context, *result);
  return result;
}

void LogContent::clear() {
  lines.clear();
}

std::string LogContent::toString() const {
  std::ostringstream result;
  for (const auto& line : lines) {
    result << line->toString();
  }
  return result.str();
};

void LogContent::setParent(LogContent* parent) {
  this->parent = parent;
}

std::string LogContent::getContextForLine(size_t line_number) const {
  if (line_number >= lines.size()) {
    return {};
  }
  if (LogLine *line = lines.at(line_number).get()) {
    return line->getContext();
  }
  return {};
}

const std::string& LogContent::getContext() const {
  return context;
}
