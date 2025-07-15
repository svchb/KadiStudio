/* Copyright 2022 Karlsruhe Institute of Technology
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

#include "stringtokenizer.h"

namespace LibFramework {

std::set<std::string> StringTokenizer::tokenize(const std::string& str, const std::string& delimiter) const {
  std::set<std::string> tokens;

  std::string::size_type lastpos = str.find_first_not_of(delimiter, 0);
  std::string::size_type pos = str.find_first_of(delimiter, lastpos);

  while (std::string::npos != pos || std::string::npos != lastpos) {
    std::string token = str.substr(lastpos, pos - lastpos);
    removeSpecialCharacters(token);
    tokens.insert(token);
    lastpos = str.find_first_not_of(delimiter, pos);
    pos = str.find_first_of(delimiter, lastpos);
  }

  return tokens;
}

void StringTokenizer::removeSpecialCharacters(std::string& str) const {
  while (special_chars.find(str.front()) != std::string::npos) {
    str.erase(0, 1);
  }

  while (special_chars.find(str.back()) != std::string::npos) {
    str.erase(str.length() - 1);
  }
}

}
