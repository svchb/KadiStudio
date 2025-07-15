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

#include <properties/data/properties.h>

#include "colorcontrolproperty.h"

ColorControlProperty::ColorControlProperty(const std::string& name, float min, float max)
    : PropertiesModel("ColorControl", name) {

  colormap_property = addProperty(new ColorMapProperty());
  colormin_property = addProperty(new FloatProperty("colormin", min));
  colormax_property = addProperty(new FloatProperty("colormax", max));
  colormax_property->updateHint()->setEntry("singleStep", 0.01f);
  isblending_property = addProperty(new BoolProperty("alpha.enabled", false));
  alpha_property = addProperty(new FloatProperty("alpha.value", 0.5));

  updateHint("alpha.value")
            ->setEntry("speed", 1.f)
            ->setEntry("float_limit_min", 0.f)
            ->setEntry("float_limit_max", 1.f);
}

float* ColorControlProperty::getColor(float color[4], double value) {
  float colormin = colormin_property->getValue();
  float colormax = colormax_property->getValue();
  bool isblending = isblending_property->getValue();
  float alphavalue = alpha_property->getValue();

  return colormap_property->getColor(color, value, colormin, colormax, isblending, alphavalue);
}

void ColorControlProperty::setMod(const double min, const double max) {
  auto scope = suspend();

  const float minf = min;
  setValue("colormin", minf);
  const float maxf = max;
  setValue("colormax", maxf);
}

double ColorControlProperty::getModMin() const {
  return colormin_property->getValue();
}

double ColorControlProperty::getModMax() const {
  return colormax_property->getValue();
}

void ColorControlProperty::setBlending(const bool active) {
  isblending_property->setValue(active);
}

bool ColorControlProperty::isBlendingActive() const {
  return isblending_property->getValue();
}

void ColorControlProperty::setAlphaChannel(double alpha) {
  alpha_property->setValue(alpha);
  if (isBlendingActive()) {
    setDirty(true);
  }
}

double ColorControlProperty::getAlphaChannel() const {
  return alpha_property->getValue();
}
