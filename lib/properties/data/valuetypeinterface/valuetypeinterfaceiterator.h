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

#include <iterator>

#include "../valuetypeinterface/valuetypeinterfacecontainer.h"

/**
 * @brief      A custom iterator for AbstractValueTypeIterator, used for iterating values of
 *             ValueTypeInterfaceContainers.
 * @ingroup    valuetypeinterface
 *
 * Code from https://www.internalpointers.com/post/writing-custom-iterators-modern-cpp was used as a reference.
 */
class ValueTypeInterfaceIterator {

public:
  using iterator_category = std::forward_iterator_tag;
  using difference_type = std::ptrdiff_t;
  using value_type = AbstractValueTypeInterface;
  using pointer = value_type*;
  using reference = value_type&;

  explicit ValueTypeInterfaceIterator(size_t key, ValueTypeInterfaceContainer* container)
      : key(key), container(container) {}

  pointer operator*() {
    return container->getElementVTI(key);
  }

  pointer operator->() {
    return container->getElementVTI(key);
  }

  ValueTypeInterfaceIterator& operator++() {
    key++;
    return *this;
  }

  const ValueTypeInterfaceIterator operator++(int) {
    ValueTypeInterfaceIterator tmp = *this;
    ++(*this);
    return tmp;
  }

  friend bool operator==(const ValueTypeInterfaceIterator &lhs, const ValueTypeInterfaceIterator &rhs) {
    return (lhs.key == rhs.key) && (lhs.container == rhs.container);
  }

  friend bool operator!=(const ValueTypeInterfaceIterator &lhs, const ValueTypeInterfaceIterator &rhs) {
    return !(lhs == rhs);
  }

  private:
    size_t key;
    ValueTypeInterfaceContainer *container;
};
