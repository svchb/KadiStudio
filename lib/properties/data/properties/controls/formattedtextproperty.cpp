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

#include "../primitive/stringproperty.h"
#include "stringlistelementproperty.h"

#include "formattedtextproperty.h"

FormattedTextProperty::FormattedTextProperty(const std::string &property_name)
    : PropertiesModel("FormattedText", property_name) {

  text   = addProperty(new StringProperty("text", ""));
  format = addProperty(new StringListElementProperty("format", "markdown", {"markdown", "html"}));
}

std::string FormattedTextProperty::toString() const {
  std::string selectedFormat;
  format->getValueWithPath({"selected"}, selectedFormat);
  return "[" + selectedFormat + "] " + text->getValue();
}

void FormattedTextProperty::fromString(const std::string &str) {
  unsigned long splitPos = str.find("] ");
  std::string formatStr = str.substr(1, splitPos - 1);
  format->setValueWithPath({"selected"}, formatStr);
  text->setValue(str.substr(splitPos + 2));
}
