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

#include "boundingboxcontrolproperty.h"

BoundingBoxControlProperty::BoundingBoxControlProperty(const std::string& property_name, long left, long right, long bottom, long top, long back, long front)
    : PropertiesModel("BoundingBox", property_name) {

  sides[0] = addProperty(new LongProperty("left",   left     ));
  sides[1] = addProperty(new LongProperty("right",  right - 1));
  sides[2] = addProperty(new LongProperty("bottom", bottom   ));
  sides[3] = addProperty(new LongProperty("top",    top   - 1));
  sides[4] = addProperty(new LongProperty("back",   back     ));
  sides[5] = addProperty(new LongProperty("front",  front - 1));

  sides[0]->updateHint()->setEntry("limit_min", left);
  sides[0]->updateHint()->setEntry("limit_max", right - 1);
  sides[1]->updateHint()->setEntry("limit_min", left);
  sides[1]->updateHint()->setEntry("limit_max", right - 1);
  sides[2]->updateHint()->setEntry("limit_min", bottom);
  sides[2]->updateHint()->setEntry("limit_max", top   - 1);
  sides[3]->updateHint()->setEntry("limit_min", bottom);
  sides[3]->updateHint()->setEntry("limit_max", top   - 1);
  sides[4]->updateHint()->setEntry("limit_min", back);
  sides[4]->updateHint()->setEntry("limit_max", front - 1);
  sides[5]->updateHint()->setEntry("limit_min", back);
  sides[5]->updateHint()->setEntry("limit_max", front - 1);
}
