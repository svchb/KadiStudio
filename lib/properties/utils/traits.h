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
 * @file       traits.h
 * @brief      Collection of useful traits for template meta programming
 * @ingroup    utils
 *
 * Inspired by
 * @url https://stackoverflow.com/questions/22758291/how-can-i-detect-if-a-type-can-be-streamed-to-an-stdostream
 *
 */

#pragma once

#include <sstream>


/** @brief Check for reading from ios
  */
template<typename IOS, typename T>
class can_read_from_ios {
  protected:
    static_assert (std::is_base_of<std::ios, IOS>::value);

    template<typename ios, typename V>
    static auto test(V*) -> decltype(std::declval<ios&>() >> std::declval<V&>(), std::true_type());

    template<typename, typename>
    static auto test(...) -> std::false_type;

  public:
    static constexpr bool value = decltype(test<IOS, T>(nullptr))::value;
};

template<typename T>
class has_istream_operator : public can_read_from_ios<std::istream, T> {};

/** @brief Check for writing to ios
  */
template<typename IOS, typename T>
class can_write_to_ios {
  protected:
    static_assert (std::is_base_of<std::ios, IOS>::value);

    template<typename ios, typename V>
    static auto test(V*) -> decltype(std::declval<ios&>() << std::declval<V&>(), std::true_type());

    template<typename, typename>
    static auto test(...) -> std::false_type;

  public:
    static constexpr bool value = decltype(test<IOS, T>(nullptr))::value;
};

template<typename T>
class has_ostream_operator : public can_write_to_ios<std::ostream, T> {};
