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

#include <vector>

/**
 * @brief      A mixin which properties can use if they need customizable
 *             delimiters for their string representations.
 *             The property mixin adds "," as default delimiter,
 *             as well as a protected method getDelimiter() for use in toString/
 *             fromString implementations.
 *             The mixin for the specialized property interface adds an
 *             interface method to add a custom delimiter.
 * @ingroup    container
 */
class CustomDelimiterInterfaceMixin {
  public:
    virtual ~CustomDelimiterInterfaceMixin() = default;

    virtual void addDelimiter(char delimiter) = 0;
    virtual char getDelimiter(size_t index) const = 0;
};

class CustomDelimiterMixin : CustomDelimiterInterfaceMixin {
  public:
    CustomDelimiterMixin() = default;
    virtual ~CustomDelimiterMixin() = default;

  protected:
    char getDelimiter(size_t index) const override {
      char result = DEFAULT_DELIMITER;
      if (index < delimiters.size()) {
        result = delimiters.at(index);
      }
      return result;
    }

    void addDelimiter(char delimiter) override {
      delimiters.emplace_back(delimiter);
    }

    std::vector<char> delimiters;
    static const char DEFAULT_DELIMITER = ',';
};
