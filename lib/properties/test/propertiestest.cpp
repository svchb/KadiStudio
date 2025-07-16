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
 * @file       propertiestest.cpp
 * @brief      Basic set of tests for property implementations and their notify mechanisms
 * @ingroup    test
 */

#include <vector>
#include <unordered_map>

#include "propertiestest.h"

void PropertiesTest::test_arrayProperty() {
  bool signal_called = false;
  std::array<int, 4> data { -50, 8, 2, 787 };
  ArrayProperty<int, 4> testproperty("testProperty", data);
  testproperty.onValueChange([&signal_called](const std::array<int, 4>&) {
    signal_called = true;
  }).release();

  assert(testproperty.at(0) == -50);
  testproperty.setValue({0, 1, 3, -14});
  assert(signal_called);
  signal_called = false;

  auto elementVTI = testproperty.getElementVTI(2);
  dynamic_cast<ValueTypeInterface<int>*>(elementVTI)->setValue(7);
  delete elementVTI;

  assert(testproperty.at(2) == 7);
  assert(signal_called);

  assert(testproperty.toString() == "(0,1,7,-14)");
  std::string testvec = "(-3,100,50,0)";
  testproperty.fromString(testvec);
  assert(testproperty.toString() == testvec);

  auto setterfunction = [](ArrayProperty<int, 4>* property) {
    property->setValue(std::array<int, 4>{1, 2, 3, 4});
  };
  TestListener::assertPropertyNotifies<ArrayProperty<int, 4>>(setterfunction, "test", data);
  TestListener::assertPropertyNotifies<ArrayProperty<int, 4>>([](ArrayProperty<int, 4>* property) {
    auto elementVTI = dynamic_cast<ValueTypeInterface<int>*>(property->getElementVTI(0));
    elementVTI->setValue(5);
    delete elementVTI;
  }, "test", data);
  std::cout << "\tArrayProperty... OK" << std::endl;
}

void PropertiesTest::test_vectorProperty() {
  bool signal_called = false;
  std::vector<int> data { -50, 8, 2, 787 };
  VectorProperty<int> testproperty("testProperty", data);
  testproperty.onValueChange([&signal_called](const std::vector<int>) {
    signal_called = true;
  }).release();

  assert(testproperty.at(0) == -50);
  testproperty.setValue({0, 1, 3, -14});
  assert(signal_called);
  signal_called = false;

  auto elementVTI = testproperty.getElementVTI(2);
  dynamic_cast<ValueTypeInterface<int>*>(elementVTI)->setValue(7);
  delete elementVTI;

  assert(testproperty.at(2) == 7);
  assert(signal_called);
  signal_called = false;

  testproperty.sort();
  assert(signal_called);
  assert(testproperty.at(0) == -14);
  signal_called = false;

  testproperty.push_back(11);
  assert(signal_called);
  assert(testproperty.at(testproperty.size() - 1) == 11);
  signal_called = false;

  auto first = testproperty.getIteratorBegin();
  testproperty.erase(&first);
  assert(signal_called);
  assert(testproperty.at(0) == 0);
  signal_called = false;

  assert(testproperty.toString() == "(0,1,7,11)");
  std::string testvec = "(-3,100,50,0)";
  testproperty.fromString(testvec);
  assert(testproperty.toString() == testvec);

  auto setterfunction = [](VectorProperty<int>* property) {
    property->setValue(std::vector<int> {1, 2, 3, 4});
  };
  TestListener::assertPropertyNotifies<VectorProperty<int>>(setterfunction, "test", data);
  TestListener::assertPropertyNotifies<VectorProperty<int>>([](VectorProperty<int>* property) {
    auto elementVTI = dynamic_cast<ValueTypeInterface<int>*>(property->getElementVTI(0));
    elementVTI->setValue(5);
    delete elementVTI;
  }, "test", data);

  std::cout << "\tVectorProperty... OK" << std::endl;
}

void PropertiesTest::test_mapProperty() {
  bool signal_called = false;
  std::unordered_map<std::string, int> data {{"A", -50}, {"B", 8}, {"C", 2}, {"D", 787}};
  MapProperty<std::string, int> testproperty("testProperty", data);
  testproperty.onValueChange([&signal_called](const std::unordered_map<std::string, int>) {
    signal_called = true;
  }).release();

  assert(testproperty.at("A") == -50);
  testproperty.setValue({{"Z", -1}, {"X", 255}});
  assert(signal_called);
  signal_called = false;

  auto elementVTI = testproperty.getElementVTI(1);
  dynamic_cast<ValueTypeInterface<int>*>(elementVTI)->setValue(7);
  delete elementVTI;

  assert(testproperty.at("Z") == 7);
  assert(signal_called);
  signal_called = false;

  // overwrite existing entry
  testproperty.set("Z", 11);
  assert(signal_called);
  assert(testproperty.at("Z") == 11);
  signal_called = false;

  // add new entry
  testproperty.set("P", -34);
  assert(signal_called);
  assert(testproperty.at("P") == -34);
  signal_called = false;

  assert(testproperty.contains("Z"));
  testproperty.remove("Z");
  assert(signal_called);
  assert(!testproperty.contains("Z"));
  signal_called = false;

  std::string repr = "{P:-34,X:255}";
  assert(testproperty.toString() == repr);
  testproperty.clear();
  assert(signal_called);
  signal_called = false;
  testproperty.fromString(repr);

  // we can not simply compare using `assert(testproperty.toString() == repr)` because the ordering might be different
  assert(testproperty.contains("P"));
  assert(testproperty.contains("X"));
  assert(testproperty.at("P") == -34);
  assert(testproperty.at("X") == 255);
  assert(signal_called);
  signal_called = false;

  auto setterfunction = [](MapProperty<std::string, int>* property) {
    property->setValue(std::unordered_map<std::string, int> {{"test", 3}});
  };
  TestListener::assertPropertyNotifies<MapProperty<std::string, int>>(setterfunction, "test", data);
  TestListener::assertPropertyNotifies<MapProperty<std::string, int>>([](MapProperty<std::string, int>* property) {
    auto elementVTI = dynamic_cast<ValueTypeInterface<int>*>(property->getElementVTI(0));
    elementVTI->setValue(5);
    delete elementVTI;
  }, "test", data);

  std::cout << "\tMapProperty... OK" << std::endl;
}

void PropertiesTest::test_matrixProperty() {
  using matrix_t = MatrixProperty<int>::matrix_t;
  bool signal_called = false;
  matrix_t data{{0, 1}, {2, 3}};
  MatrixProperty<int> testproperty("testProperty", true, data);
  testproperty.onValueChange([&signal_called](const matrix_t&) {
    signal_called = true;
  }).release();

  testproperty.setValue({{0, 1, 2}, {3, 4, 5}});
  assert(signal_called);
  signal_called = false;

  auto elementVTI = testproperty.getElementVTI(1, 0);
  dynamic_cast<ValueTypeInterface<int>*>(elementVTI)->setValue(7);
  delete elementVTI;

  assert(testproperty.at(1, 0) == 7);
  assert(signal_called);
  signal_called = false;

  testproperty.addRow(std::vector<int> {6, 7, 8});
  assert(signal_called);
  assert(testproperty.at(2, 2) == 8);
  signal_called = false;

  std::string repr = "[(0,1,2),(7,4,5),(6,7,8)]";
  assert(testproperty.toString() == repr);
  testproperty.clear();
  assert(signal_called);
  signal_called = false;
  testproperty.fromString(repr);
  assert(testproperty.toString() == repr);
  assert(signal_called);
  signal_called = false;

  auto setterfunction = [](MatrixProperty<int>* property) {
    property->setValue(MatrixProperty<int>::matrix_t {{0, 1}, {7, 6}});
  };
  TestListener::assertPropertyNotifies<MatrixProperty<int>>(setterfunction, "test", data);
  TestListener::assertPropertyNotifies<MatrixProperty<int>>([](MatrixProperty<int>* property) {
    auto elementVTI = property->getElementVTI(0);
    auto elementVTI2 = dynamic_cast<ElementVectorTypeInterface<matrix_t, MatrixProperty<int>::vector_t, int>*>(elementVTI)->getElementVTI(0);
    dynamic_cast<ValueTypeInterface<int>*>(elementVTI2)->setValue(5);
    delete elementVTI;
  }, "test", data);

  std::cout << "\tMatrixProperty... OK" << std::endl;
}

void PropertiesTest::test_dynamicProperty() {
  bool signal_called = false;
  DynamicProperty testproperty("myTestProperty");
  testproperty.onValueChange([&signal_called](const std::vector<AbstractValueTypeInterface*>) {
    signal_called = true;
  }).release();

  std::vector<AbstractValueTypeInterface*> data {new ValueType<bool>(true)};
  testproperty.setValue(data);
  assert(signal_called);
  signal_called = false;

  auto elementVTI = testproperty.getElementVTI(0);
  // TODO make it possible to change the value of the value (AbstractVTI*) using the ElementVTI (instead of replacing it) ?
  dynamic_cast<ElementTypeInterface<std::vector<AbstractValueTypeInterface*>, AbstractValueTypeInterface*>*>(elementVTI)->setValue(new ValueType<bool>(false));
  delete elementVTI;

  assert(dynamic_cast<ValueTypeInterface<bool>*>(testproperty.at(0))->getValue() == false);
  assert(signal_called);
  signal_called = false;

  testproperty.addValue(new ValueType<int>(7));
  assert(signal_called);
  testproperty.addValue(new ValueType<float>(43.3));
  testproperty.addValue(new ValueType<std::string>("this is a test"));
  signal_called = false;

  // TODO also test with other delimiters maybe
  std::string repr = "0,7,43.3,this is a test"; // TODO make sure strings don't contain ','
  assert(testproperty.toString() == repr);
  // testproperty.clear();
  // assert(signal_called);
  signal_called = false;
  testproperty.fromString(repr);
  assert(testproperty.toString() == repr);
  assert(signal_called);
  signal_called = false;

  auto setterfunction = [](DynamicProperty* property) {
    property->setValue(std::vector<AbstractValueTypeInterface*> {new ValueType<bool>(true)});
  };
  TestListener::assertPropertyNotifies<DynamicProperty>(setterfunction, "test", std::vector<AbstractValueTypeInterface*> {new ValueType<bool>(true)});
  TestListener::assertPropertyNotifies<DynamicProperty>([](DynamicProperty* property) {
    auto elementVTI = property->getElementVTI(0);
    dynamic_cast<ValueTypeInterface<AbstractValueTypeInterface*>*>(elementVTI)->setValue(new ValueType<bool>(false));
    delete elementVTI;
  }, "test", std::vector<AbstractValueTypeInterface*> {new ValueType<bool>(true)});

  std::cout << "\tDynamicProperty... OK" << std::endl;
}

int main() {
  std::cout << "Running basic tests on property implementations..." << std::endl;
  PropertiesTest().run();
  std::cout << "All test OK" << std::endl;
}
