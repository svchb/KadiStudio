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

#include <iostream>
#include <iomanip>
#include <array>
#include <string>

#include "../primitive/stringproperty.h"


class ColorChooserProperty : public StringProperty {

  public:
    ColorChooserProperty(const std::string& name, const std::string& defaultcolor = "FFFFFF")
        : StringProperty(name, defaultcolor) {
      updateHint()->setWidgetIdentifier("colorChooser");
    }

    void setColor(const float color[3]) {
      std::stringstream stream;

      int combined = ((int)(color[0]*255) << 16) | ((int)(color[1]*255) << 8) | (int)(color[2]*255);
      stream << std::uppercase << std::hex << std::setw(6) << std::setfill('0') << combined;

      setValue(stream.str());
    }

    void getColor(float color[3]) {
      std::string colorstring;
      getValue(colorstring);

      size_t pos;
      unsigned int colorvalue;
      try {
        colorvalue = std::stoul(colorstring, &pos, 16);
      } catch (std::exception const& ex) {
        std::cerr << "Could not convert '" << colorstring << "' to a color (" << ex.what() << ")" << std::endl;
      }
      if (pos != colorstring.size()) { // string not valid
        // std::cerr << "String '" << colorstring << "' is not a valid hex color!" << std::endl;
        // If the color can not be parsed, set the color to black to avoid
        // setting a 'random' color.
        color[0] = 0.0;
        color[1] = 0.0;
        color[2] = 0.0;
      } else {
        color[0] = (float)((colorvalue >> 16) & 0x0000FF) / 255.0; // red
        color[1] = (float)((colorvalue >>  8) & 0x0000FF) / 255.0; // green
        color[2] = (float)((colorvalue >>  0) & 0x0000FF) / 255.0; // blue
      }
    }

    void getColor(std::array<float, 3>& color) {
      getColor(color.data());
    }

  private:

};
