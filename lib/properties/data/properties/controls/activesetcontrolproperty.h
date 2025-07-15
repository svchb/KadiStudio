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

#include <regex>
#include <stdexcept>

#include "../propertyvti.h"
#include "../../valuetypeinterface/valuetypeinterface.h"


class ActiveSetVTI : public ValueType<std::pair<std::vector<std::string>, std::vector<std::string>>> {

  public:
    ActiveSetVTI(const std::pair<std::vector<std::string>, std::vector<std::string>>& elementsset)
        : ValueType<std::pair<std::vector<std::string>, std::vector<std::string>>>(elementsset) {

      updateHint()->setWidgetIdentifier("ActiveSetControl");

      sort(activeElements());
      sort(deactiveElements());

    }

    ActiveSetVTI(const std::vector<std::string>& activeelementnames,
                 const std::vector<std::string>& deactiveelementnames)
        : ActiveSetVTI(std::pair(activeelementnames, deactiveelementnames)) {}

    virtual ~ActiveSetVTI() = default;

    /** @brief checks if the element is active.
      *
      * @return false if the element is not in the active set or is not defined in the whole set, else true.
      */
    bool isActive(const std::string& elementname) {
      return std::binary_search(activeElements().begin(), activeElements().end(), elementname);
    }

    /** @brief checks it the element is inactive.
      *
      * @return false if the element is not in the deactive set or is not defined in the whole set, else true.
      */
    bool isDeactive(const std::string& elementname) {
      return std::binary_search(deactiveElements().begin(), deactiveElements().end(), elementname);
    }

    void setElements(std::vector<std::string> elements, bool elementsAreActivated = true) {
      sort(elements);
      if (elementsAreActivated) {
        setValue({elements, {}});
      } else {
        setValue({{}, elements});
      }
    }

    const std::vector<std::string>& getActiveElements() const {
      return value.first;
    }

    const std::vector<std::string>& getDeactiveElements() const {
      return value.second;
    }

    void activateElement(const std::string& elementname) {
      auto elmiter = std::find(deactiveElements().begin(), deactiveElements().end(), elementname);
      if (elmiter != deactiveElements().end()) {
        deactiveElements().erase(elmiter);
        activeElements().push_back(elementname);
        sort(activeElements());

        notify(value);
      }
    }

    void deactivateElement(const std::string& elementname) {
      auto elmiter = std::find(activeElements().begin(), activeElements().end(), elementname);
      if (elmiter != activeElements().end()) {
        activeElements().erase(elmiter);
        deactiveElements().push_back(elementname);
        sort(deactiveElements());

        notify(value);
      }
    }

    void activateAllElements() {
      std::vector<std::string> activeelementnames;
      activeelementnames.reserve(activeElements().size() + deactiveElements().size());
      std::merge(std::make_move_iterator(value.first.begin()), std::make_move_iterator(value.first.end()),
                 std::make_move_iterator(value.second.begin()), std::make_move_iterator(value.second.end()),
                 std::back_inserter(activeelementnames));
      setValue({sort(activeelementnames), {}});
    }

    void deactivateAllElements() {
      std::vector<std::string> deactiveelementnames;
      deactiveelementnames.reserve(activeElements().size() + deactiveElements().size());
      std::merge(std::make_move_iterator(value.first.begin()), std::make_move_iterator(value.first.end()),
                 std::make_move_iterator(value.second.begin()), std::make_move_iterator(value.second.end()),
                 std::back_inserter(deactiveelementnames));
      setValue({{}, sort(deactiveelementnames)});
    }

    template<typename conditionFunc>
    void activateElementsByCondition(conditionFunc func) {
      for (auto iter = deactiveElements().begin(); iter != deactiveElements().end();) {
        if (func(*iter)) {
          activeElements().push_back(*iter);
          deactiveElements().erase(iter);
        } else {
          ++iter;
        }
      }
      sort(activeElements());

      notify(value);
    }

    template<typename conditionFunc>
    void deactivateElementsByCondition(conditionFunc func) {
      for (auto iter = activeElements().begin(); iter != activeElements().end();) {
        if (func(*iter)) {
          deactiveElements().push_back(*iter);
          activeElements().erase(iter);
        } else {
          ++iter;
        }
      }
      sort(deactiveElements());

      notify(value);
    }

    void clear() {
      value.first.clear();
      value.second.clear();

      notify(value);
    }

  private:

    std::vector<std::string>& activeElements() {
      return value.first;
    }

    std::vector<std::string>& deactiveElements() {
      return value.second;
    }

    std::vector<std::string>& sort(std::vector<std::string>& vector) {
      std::sort(vector.begin(), vector.end(), [](const std::string& left, const std::string& right) {
        return left < right;
      });
      return vector;
    }

  protected:

    void fromString(const std::string& str) override {
      std::vector<std::string> activeelementnames;
      std::vector<std::string> deactiveelementnames;

      std::regex rgx("\\{((?:\\{(?:[^\\}])*\\})*)\\};\\{((?:\\{(?:[^}])*\\})*)\\}");
      std::regex rgx_elem("(?:\\{([^\\}]*)\\})");

      std::smatch matches;

      if (std::regex_search(str, matches, rgx) && matches.size() == 3) {

        for (size_t i = 1; i < matches.size(); ++i) {

          std::string str_elem = matches[i].str();

          std::smatch matches_elems;
          if (std::regex_search(str_elem, matches_elems, rgx_elem)) {
            std::sregex_iterator j = std::sregex_iterator(str_elem.begin(), str_elem.end(), rgx_elem);
            for (; j != std::sregex_iterator(); ++j) {
              std::string token = *(++((*j).begin())); // first is the total match, so we want the second match
              if (i==1) {
                activeelementnames.push_back(token);
              } else {
                deactiveelementnames.push_back(token);
              }
            }
          } else {
            throw std::runtime_error("Could not retrive pair set from string");
          }
        }
        setValue({activeelementnames, deactiveelementnames});
      } else {
        throw std::runtime_error("Could not retrive pair set from string");
      }
    }

    std::string toString() const override {
      std::string result = "{";
      for (std::string elem : getActiveElements()) {
        result += "{";
        result += elem;
        result += "}";
      }
      result += "};{";
      for (std::string elem : getDeactiveElements()) {
        result += "{";
        result += elem;
        result += "}";
      }
      result += "}";
      return result;
    }

};

class ActiveSetControlProperty : public PropertyVTI<ActiveSetVTI> {

  public:
    ActiveSetControlProperty(std::string name,
                             const std::vector<std::string>& activeelementnames,
                             const std::vector<std::string>& deactiveelementnames, bool makesdirty = true)
        : PropertyVTI<ActiveSetVTI>(name, makesdirty, std::pair(activeelementnames, deactiveelementnames)) {}

    virtual ~ActiveSetControlProperty() = default;

};
