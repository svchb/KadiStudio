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

#include <unordered_map>
#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>

#include "../../../qpropertywidget.h"

class QPushButton;
class QTextEdit;

enum ElementType {
  ACTINIDE,
  ALKALI_METAL,
  ALKALINE_EARTH_METAL,
  DIATOMIC_NONMETAL,
  LANTHANIDE,
  METALLOID,
  NOBLE_GAS,
  POLYATOMIC_NONMETAL,
  POST_TRANSITION_METAL,
  TRANSITION_METAL,
  UNKNOWN
};

struct Element {
  std::string name;
  ElementType type;
  unsigned int xPos;
  unsigned int yPos;
};

/**
 * @brief      A widget which allows to select multiple elements from the periodic table
 * @ingroup    qtwidgetpropertyfactory
 */
class PeriodicTableWidget : public QPropertyWidget {
  Q_OBJECT

  public:
    PeriodicTableWidget(Property* property, QWidget* parent = nullptr);
    ~PeriodicTableWidget() = default;

    void synchronizeVTI() override;

  private:
    void setValue(const std::vector<std::string>& value);
    void updateValue();
    void setElementButtonState(const std::string& elementName, bool checked);
    std::set<std::string> selectedElements;
    std::unordered_map<std::string, QPushButton*> elementButtons;

    const static int ELEMENT_SIZE;
    const static int TEXTEDIT_HEIGHT;
    const static std::vector<Element> elements;
    const static std::unordered_map<ElementType, QString> elementTypeColors;

    void setElementSelected(const std::string& elementName, bool checked);
    void clearSelection();

    QTextEdit *valueText;
};
