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


class ColorControlProperty : public PropertiesModel {

  public:
    ColorControlProperty(const std::string& name, float min = 0.0f, float max = 0.0f);

    virtual ~ColorControlProperty() = default;

    void setMod(double min, double max);

    double getModMin() const;
    double getModMax() const;

    void setBlending(bool active);
    bool isBlendingActive() const;

    /** value from 0..1
      */
    void setAlphaChannel(double alpha);
    double getAlphaChannel() const;

    float* getColor(float color[4], double value);

  private:
    FloatProperty *colormin_property;
    FloatProperty *colormax_property;
    BoolProperty *isblending_property;
    FloatProperty *alpha_property;

    ColorMapProperty *colormap_property;

};
