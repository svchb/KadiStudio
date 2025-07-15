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

/**
 * @file       defaults.h
 * @brief      Reusable default values for composing properties
 * @ingroup    utils
 */

#include <array>

namespace defaults {
  /**
   * indices of the modelmatrix
   *
   *|  0  4  8 12 |
   *|  1  5  9 13 |
   *|  2  6 10 14 |
   *|  3  7 11 15 |
   */
  const std::array<float, 16> identity_rotation = {
    1.0, 0.0, 0.0, 0.0,
    0.0, 1.0, 0.0, 0.0,
    0.0, 0.0, 1.0, 0.0,
    0.0, 0.0, 0.0, 1.0,
  };
}
