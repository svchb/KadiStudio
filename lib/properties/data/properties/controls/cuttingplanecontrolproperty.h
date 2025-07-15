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

#include "../../propertiesmodel.h"

class CuttingPlaneControlProperty : public PropertiesModel {

  public:
    CuttingPlaneControlProperty(long cutdirection, int nx, int ny, int nz)
        : PropertiesModel("CuttingPlaneControl") {

      addProperty(new BoundingBoxControlProperty("boundingbox", 0, nx, 0, ny, 0, nz));

      addProperty(new LongProperty("cut.direction", cutdirection));

      int position[3] = { nx, ny, nz };

      addProperty(new LongProperty("cut.position", position[cutdirection] / 2));
    }

    void setBound(const std::string& side, long value) {
      setValueWithPath({std::string("boundingbox"), side}, value);
    }

    long getBound(const std::string& side) const {
      return getValueWithPath<long>({std::string("boundingbox"), side});
    }
};
