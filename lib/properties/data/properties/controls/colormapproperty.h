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

#include "properties/data/propertiesmodel.h"
struct colormap_s;


class ColorMapProperty : public PropertiesModel {

  public:

    ColorMapProperty();
    ~ColorMapProperty();

    void setColorMap(struct colormap_s* const& value);
    struct colormap_s* const& getColorMap();

    void setColorMode(const std::string& value);
    const std::string& getColorMode();
    void setColorNegation(const bool& value);
    const bool& getColorNegation();
    void setColorInvertion(const bool& value);
    const bool& getColorInvertion();
    void setConstColor(const std::string& value);
    void setConstColor(unsigned char red, unsigned char green, unsigned char blue);
    const std::string& getConstColor();
    void getConstColor(unsigned char& red, unsigned char& green, unsigned char& blue);
    void setColorFile(const std::string& value);
    const std::string& getColorFile();

    float* getColor(float rgba[4], double value, float colormin, float colormax, bool isblending, float alpha);
    void getColorNormalized(unsigned char color[4], double value);

  private:

    void updateColorModes();

    struct colormap_s *colormap;
    std::string filename;

    std::string tmp_paceColorString;
    std::string tmp_constcolor;

};
