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

#include "../primitive/longproperty.h"
#include "../primitive/stringproperty.h"

#include "cropselectionproperty.h"

CropSelectionProperty::CropSelectionProperty(const std::string &property_name)
    : PropertiesModel("CropSelection", property_name) {

  imagepath = addProperty(new StringProperty("imagePath", ""));
  // -1 as a symbol for "not set"
  xposition = addProperty(new LongProperty("x", -1));
  yposition = addProperty(new LongProperty("y", -1));
  width     = addProperty(new LongProperty("width", -1));
  height    = addProperty(new LongProperty("height", -1));

}

std::string CropSelectionProperty::toString() const {
  if (xposition->getValue() != -1 || yposition->getValue() != -1 || width->getValue() != -1 || height->getValue() != -1) {
    std::ostringstream oss;
    oss << "[" << xposition->getValue() << ", " << yposition->getValue() << ", " << width->getValue() << ", "
        << height->getValue() << "]";
    return oss.str();
  } else {
    return {};
  }
}

void CropSelectionProperty::fromString(const std::string &value) {
  std::string delimiter = ",";
  std::string s = value.substr(1);
  auto propertyFromNextInt = [&s, &delimiter] (LongProperty *property) {
    size_t pos = 0;
    std::string token;
    if ((pos = s.find(delimiter)) != std::string::npos) {
      token = s.substr(0, pos);
      property->setValue(std::stoi(token));
      s.erase(0, pos + delimiter.length());
    }
  };
  propertyFromNextInt(xposition);
  propertyFromNextInt(yposition);
  propertyFromNextInt(width);
  propertyFromNextInt(height);
}
