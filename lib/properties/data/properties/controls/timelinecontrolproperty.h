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

#include "../primitive/longproperty.h"
#include "../primitive/boolproperty.h"
#include "../primitive/floatproperty.h"
#include "../primitive/stringproperty.h"
#include "properties/data/propertiesmodel.h"

class TimeLineControlProperty : public PropertiesModel {

  public:
    TimeLineControlProperty(long maxtimesteps, float delay, const std::string& selected, const std::initializer_list<std::string> &timelines)
        : PropertiesModel("TimeLine") {

      addProperty(new LongProperty("timestep", 0))
                       ->updateHint()->setEntry("limit_max", maxtimesteps);
      addProperty(new FloatProperty("pausetime", 0.0f))
                       ->updateHint()->setEntry("limit_min", 0.0f);
      addProperty(new FloatProperty("delay", delay))
                       ->updateHint()->setEntry("limit_min", 0.0f);
      addProperty(new BoolProperty("repeat", false));
      addProperty(new BoolProperty("record", false));
      addProperty(new StringProperty("timelines", selected))
                       ->updateHint()->setValidator<ListValidator<std::string>>(ValidatorType::IN_LIST, timelines);
    }
};
