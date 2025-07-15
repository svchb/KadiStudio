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

/**
 * @file       sizeinfo.h
 * @brief      Testprogram to compare sizes of property classes with sizes of other types
 * @ingroup    test
 */

#pragma once

#include <numeric>
#include <properties/data/properties.h>

struct compareSize_t {
  std::string description;
  size_t typeA;
  size_t typeB;
  int diff;
};

void printRow(const compareSize_t& compareSize, const std::array<unsigned int, 4> columnWidths, std::string overwriteDescription = "");

template <class T>
std::string typeName() {
  std::string result = demangle(typeid(T).name());
  replaceAll(result, typeid(std::string).name(), "std::string");
  return result;
}

template <typename PROPERTY, typename TYPE>
compareSize_t compareSize() {
  compareSize_t result;
  result.description = typeName<PROPERTY>() + " (" + typeName<TYPE>() + ")";
  result.typeA = sizeof(PROPERTY);
  result.typeB = sizeof(TYPE);
  result.diff = result.typeA - result.typeB;
  return result;
}

template <unsigned int N>
void printSeparator(const std::array<unsigned int, N>& columnWidths) {
  std::cout << std::string(std::accumulate(columnWidths.begin(), columnWidths.end(), 0), '-') << std::endl;
}
