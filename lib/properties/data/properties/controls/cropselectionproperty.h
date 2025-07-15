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

#include <string>
#include "properties/data/propertiesmodel.h"

class LongProperty;
class StringProperty;

class CropSelectionProperty : public PropertiesModel {

  public:
    explicit CropSelectionProperty(const std::string &property_name);
    CropSelectionProperty(const CropSelectionProperty&) = delete;
    CropSelectionProperty& operator=(const CropSelectionProperty&) = delete;

    ~CropSelectionProperty() override = default;

    std::string toString() const override;
    void fromString(const std::string& value) override;

  private:
    StringProperty *imagepath;
    LongProperty *xposition;
    LongProperty *yposition;
    LongProperty *width;
    LongProperty *height;
};
