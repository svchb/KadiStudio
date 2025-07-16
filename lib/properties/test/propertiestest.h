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
 * @file       propertiestest.h
 * @brief      Basic set of tests for property implementations and their notify mechanisms
 * @ingroup    test
 */

#pragma once

#include <iostream>
#include <string>
#include <cassert>

#include <properties/data/properties.h>

class TestListener : public PropertyChangeListener { // TODO PropertyChangeListener entfernen
  public:
    TestListener(const Property* expected_property)
        : expected_property(expected_property), wasCalled(false) {}

    template <typename PROPERTY, typename... ConstructorArgs>
    static void assertPropertyNotifies(const std::function<void(PROPERTY*)> changePropertyFunctor, ConstructorArgs... args) {
      static_assert(std::is_base_of<Property, PROPERTY>::value, "This test function only works for properties.");
      Property *property = new PROPERTY(args...); // delete occurs in ~PropertiesModel()
      PropertiesModel *testModel = new PropertiesModel("test"); // delete occurs in ~Ambassador()
      testModel->addProperty(property);
      TestListener testListener(property);
      testModel->registerPropertyChangeListener(&testListener);
      changePropertyFunctor(dynamic_cast<PROPERTY*>(property));
      testListener.assertCalled();
    }

  private:
    void receivePropertyChange(const Property *property) override {
      if (expected_property == property) {
        wasCalled = true;
      }
    }

    void assertCalled() {
      assert(wasCalled);
    }

    const Property *expected_property;
    bool wasCalled;
};


class PropertiesTest {
  public:
    void run() {
      test_arrayProperty();
      test_vectorProperty();
      test_mapProperty();
      test_matrixProperty();
      test_dynamicProperty();
    }
  private:
    void test_primitiveProperties();
    void test_arrayProperty();
    void test_vectorProperty();
    void test_mapProperty();
    void test_matrixProperty();
    void test_dynamicProperty();
};
