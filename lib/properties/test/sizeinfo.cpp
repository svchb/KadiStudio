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
 * @file    sizeinfo.cpp
 * @ingroup test
 * @brief   Testprogram to compare sizes of property classes with sizes of other types
 */

#include <iostream>
#include <iomanip>
#include <cpputils/demangle.h>
#include <properties/data/properties.h>

#include "sizeinfo.h"

using namespace std;

void printRow(const compareSize_t &compareSize, const std::array<unsigned int, 4> columnWidths,
              std::string overwriteDescription) {
  std::cout << std::left << std::setw(columnWidths[0])
            << (overwriteDescription.empty() ? compareSize.description : overwriteDescription)
            << std::setw(columnWidths[1])
            << std::to_string(compareSize.typeA) << std::setw(columnWidths[2])
            << std::to_string(compareSize.typeB) << std::setw(columnWidths[3])
            << (compareSize.diff > 0 ? "+" + std::to_string(compareSize.diff) : std::to_string(compareSize.diff))
            << std::endl;
}

int main() {

  std::array<unsigned int, 4> columnWidths = {60, 18, 16, 12};
  cout << left << setw(columnWidths[0])
       << "CLASS_NAME (COMPARED TYPE)" << setw(columnWidths[1])
       << "SIZEOF CLASS" << setw(columnWidths[2])
       << "SIZEOF TYPE" << setw(columnWidths[3]) << "DIFF" << endl;
  printSeparator<4>(columnWidths);

  printRow(compareSize<ValueType<bool>, bool>(), columnWidths);
  printRow(compareSize<ValueType<int>, int>(), columnWidths);
  printRow(compareSize<ValueType<unsigned int>, unsigned int>(), columnWidths);
  printRow(compareSize<ValueType<long>, long>(), columnWidths);
  printRow(compareSize<ValueType<float>, float>(), columnWidths);
  printRow(compareSize<ValueType<double>, double>(), columnWidths);
  printRow(compareSize<ValueType<std::string>, std::string>(), columnWidths,
           "ValueInterface<std::string> (std::string)");

  printSeparator<4>(columnWidths);
  printRow(compareSize<BoolProperty, bool>(), columnWidths);
  printRow(compareSize<DoubleProperty, double>(), columnWidths);
  printRow(compareSize<FloatProperty, float>(), columnWidths);
  printRow(compareSize<IntProperty, int>(), columnWidths);
  printRow(compareSize<LongProperty, long>(), columnWidths);
  printRow(compareSize<StringProperty, std::string>(), columnWidths, "StringProperty (std::string)");
  printRow(compareSize<ULongProperty, unsigned long>(), columnWidths);

  printSeparator<4>(columnWidths);
  printRow(compareSize<PropertiesModel, Ambassador>(), columnWidths);
  printRow(compareSize<ArrayProperty<int, 3>, std::array<int, 3>>(), columnWidths);
  printRow(compareSize<ArrayProperty<int, 100>, std::array<int, 100>>(), columnWidths);
  printRow(compareSize<ArrayProperty<int, 1000>, std::array<int, 1000>>(), columnWidths);
  printRow(compareSize<ArrayProperty<int, 10000>, std::array<int, 10000>>(), columnWidths);
  printRow(compareSize<VectorProperty<int>, std::vector<int>>(), columnWidths,
           "VectorProperty<int> (std::vector<int>)");
  printRow(compareSize<MapProperty<int, int>, std::map<int, int>>(), columnWidths,
           "MapProperty<int, int> (std::map<int, int>)");

  return 0;
}
