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

#include "../../../sides.h"
#include "../primitive/longproperty.h"
#include "../../propertiesmodel.h"

class BoundingBoxControlProperty : public PropertiesModel {

  public:

    BoundingBoxControlProperty(const std::string& property_name, long left, long right, long bottom, long top, long back, long front);

    BoundingBoxControlProperty(const BoundingBoxControlProperty&) = delete;
    BoundingBoxControlProperty& operator=(const BoundingBoxControlProperty&) = delete;

    const long& getBound(Side side) const {
      return sides[side]->getValue();
    }
    const long& getBound(const std::string& side) const {
      return getProperty<LongProperty>(side)->getValue();
    }
    void getBounds(long* bounds) const {
      for (int i = 0; i < 6; i++) {
        bounds[i] = getBound(static_cast<Side>(i));
      }
    }

  private:
    LongProperty *sides[6];

};
