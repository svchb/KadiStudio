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

#include <string>
#include <vector>
#include <iostream>

#include "../../wrapper.h"
extern "C" {
  #include "../../colormap.h"
}

#include "../primitive/stringproperty.h"
#include "../delegateproperty.h"
#include "colormapproperty.h"


ColorMapProperty::ColorMapProperty() : PropertiesModel("colormap") {
  colormap = ColorMap_init(newColorMap(), NULL);

  addProperty(
    new ConverterDelegateProperty<colormap_s*, ColorMapProperty, &ColorMapProperty::setColorMap, &ColorMapProperty::getColorMap>(
      "colormap", this, [&, this](const std::string& str) -> colormap_s* {
        setColorMode(str);
        return colormap;
      }, [](colormap_s* const& value) -> std::string {
        if (!value) {
          return "";
        }
        return PaceColorStrings[value->mode];
      })
  );

  std::string value = getColorMode();
  StringProperty *colormode_property = addProperty(new StringProperty("colormode", value));
  colormode_property->onValueChange([this](const std::string& value) {
    setColorMode(value);
  }).release();

  addProperty(new DelegateProperty<bool,        ColorMapProperty, &ColorMapProperty::setColorInvertion, &ColorMapProperty::getColorInvertion>("invert", this));
  addProperty(new DelegateProperty<bool,        ColorMapProperty, &ColorMapProperty::setColorNegation, &ColorMapProperty::getColorNegation>("negate", this));
  addProperty(new DelegateProperty<std::string, ColorMapProperty, &ColorMapProperty::setConstColor, &ColorMapProperty::getConstColor>("constcolor", this));
  addProperty(new DelegateProperty<std::string, ColorMapProperty, &ColorMapProperty::setColorFile, &ColorMapProperty::getColorFile>("colorfile", this));

  updateColorModes();
}

ColorMapProperty::~ColorMapProperty() {
  ColorMap_deinit(colormap);
  delete colormap;
}

void ColorMapProperty::setColorMap(colormap_s* const& UNUSED(value)) {
//   colormap = *value;
}

colormap_s* const& ColorMapProperty::getColorMap() {
  return colormap;
}

void ColorMapProperty::setColorNegation(const bool& value) {
  colormap->negate = value;
}

const bool& ColorMapProperty::getColorNegation() {
  return colormap->negate;
}

void ColorMapProperty::setColorInvertion(const bool& value) {
  colormap->invert = value;
}

const bool& ColorMapProperty::getColorInvertion() {
  return colormap->invert;
}

void ColorMapProperty::setColorMode(const std::string& value) {
  long resolvedmode = ColorMap_resolveMode(value.c_str());
  if ((resolvedmode == -1) ||
      (resolvedmode == COLORFILE && !ColorMap_hasColorFile(colormap))) {
    return;
  }
  ColorMap_setColorMode(colormap, PaceColorMode(resolvedmode));
}

const std::string& ColorMapProperty::getColorMode() {
  tmp_paceColorString = PaceColorStrings[colormap->mode];
  return tmp_paceColorString;
}

void ColorMapProperty::updateColorModes() {
  std::vector<std::string> colormodes;
  colormodes.reserve(NUM_PACECOLORS);
  for (size_t i = 0; i < NUM_PACECOLORS; i++) {
    if (i == COLORFILE && !ColorMap_hasColorFile(colormap)) {
      continue;
    }
    colormodes.push_back(PaceColorStrings[i]);
  }

  ValueTypeInterfaceHint *hint = getProperty("colormode")->updateHint();
  hint->setValidator<ListValidator<std::string>>(ValidatorType::IN_LIST, colormodes);
}

void ColorMapProperty::setColorFile(const std::string& value) {
  filename = value;
  ColorMap_setFile(colormap, filename.c_str());
  updateColorModes();
}

const std::string& ColorMapProperty::getColorFile() {
  return filename;
}

/** @brief Extracts the rgb values out of a hexstring and converts them to char values
  *
  * @param hexcolor The hex string represnting a rgb color
  * @param rgb      The byte array to pack the values into
  */
static bool String_makeRGBb(const char* hexcolor, unsigned char rgb[3]) {
  unsigned long color;
  if (strlen(hexcolor) != 6) {
    return false;
  }

  errno = 0;
  color = strtoul(hexcolor, NULL, 16);
  if (errno != 0) return false;

  rgb[0] = (unsigned char)((color >> 16)           ); // red
  rgb[1] = (unsigned char)((color >>  8) & 0x0000FF); // green
  rgb[2] = (unsigned char)((color >>  0) & 0x0000FF); // blue

  return true;
}

void ColorMapProperty::setConstColor(const std::string& value) {
  if (not String_makeRGBb(value.c_str(), colormap->constcolor)) {
    std::cerr << "  (WW) Error bad color string." << std::endl;
  }
}

void ColorMapProperty::setConstColor(unsigned char red, unsigned char green, unsigned char blue) {
  colormap->constcolor[0] = red;
  colormap->constcolor[1] = green;
  colormap->constcolor[2] = blue;
}

const std::string& ColorMapProperty::getConstColor() {
  char buffer[7];
  snprintf(buffer, sizeof(buffer), "%2X%2X%2X", colormap->constcolor[0], colormap->constcolor[1], colormap->constcolor[2]);
  tmp_constcolor = std::string(buffer);
  return tmp_constcolor;
}

void ColorMapProperty::getConstColor(unsigned char& red, unsigned char& green, unsigned char& blue) {
  red   = colormap->constcolor[0];
  green = colormap->constcolor[1];
  blue  = colormap->constcolor[2];
}

float* ColorMapProperty::getColor(float rgba[4], const double value, float colormin, float colormax, bool isblending, float alpha) {
  unsigned char color[3];
  ColorMap_getColor(colormap, color, value,
                    colormin,
                    colormax);
  for (int i=0; i<3; i++) {
    rgba[i] = color[i]/255.0;
  }
  rgba[3] = (isblending) ? alpha : 1.0;

  return rgba;
}

void ColorMapProperty::getColorNormalized(unsigned char color[4], double value) {
  ColorMap_getColorNormalized(colormap, color, value);
}


template<>
template<>
void ValueType<colormap_s*>::fromStringInternal(const std::string& /*str*/, colormap_s*& /*value*/) const {
}
template<>
template<>
std::string ValueType<colormap_s*>::toStringInternal(colormap_s* const& /*value*/) const {
  return "";
}
