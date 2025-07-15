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

struct Keytype_s;
typedef Keytype_s Keytype;
#include <string>
#include <vector>
#include <typeinfo>

class Property;

/**
 * @brief      A factory to generate properties.
 * @ingroup    data
 */
class PropertyFactory {
  public:
    static Property* createProperty(const std::string& name, const std::string& type_name, bool isoptional, const std::string& default_value);
    static Property* createProperty(const std::string& name, Keytype* type, const std::string& default_value, const std::vector<std::string>& options);
    static Property* createProperty(const std::string& name, const std::type_info& info, const std::string& default_value, const std::vector<std::string>& options);
};
