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

#include <QPushButton>
#include <QLabel>
#include <QTextEdit>

#include "periodictablewidget.h"

PeriodicTableWidget::PeriodicTableWidget(Property* property, QWidget* parent)
    : QPropertyWidget(property, new QWidget(parent)) {
  getWidget()->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

  Ambassador *innerAmbassador = getInnerAmbassador();
  Q_ASSERT(innerAmbassador && innerAmbassador->hasProperty("selectedElements"));
  innerAmbassador->onDomChange([this]([[maybe_unused]] const std::string& path) {
    auto ambassador = getInnerAmbassador();

    // if (path == "selectedElements") {
      std::vector<std::string> selectedElements;
      ambassador->getValue("selectedElements", selectedElements);
      setValue(selectedElements);
    // }
  }).release();

  // create a subwidget for the periodic table
  auto *periodicTableWidget = new QWidget();
  auto *periodicTableLayout = new QGridLayout();
  periodicTableWidget->setLayout(periodicTableLayout);
  periodicTableLayout->setHorizontalSpacing(0);
  periodicTableLayout->setVerticalSpacing(0);
  for (int columnNumber = 1; columnNumber <= 18; columnNumber++) {
    auto *columnLabel = new QLabel(QString::number(columnNumber));
    columnLabel->setAlignment(Qt::AlignCenter);
    columnLabel->setStyleSheet("QLabel { color: #d3d3d3; }");
    columnLabel->setFixedHeight(ELEMENT_SIZE);
    periodicTableLayout->addWidget(columnLabel, 0, columnNumber);
  }
  for (const Element& element : elements) {
    auto *elementButton = new QPushButton(QString::fromStdString(element.name));
    elementButton->setCheckable(true);
    elementButton->setFixedSize(ELEMENT_SIZE, ELEMENT_SIZE);
    if (elementTypeColors.find(element.type) != elementTypeColors.end()) {
      elementButton->setStyleSheet(QString(
        "QPushButton { background-color: #%1; }"
        "QPushButton:disabled:!checked {"
        "  background-color: rgba(206, 206, 206, 0.5); " // this makes selected elements stand out when the widget is disabled
        "}"
      ).arg(elementTypeColors.at(element.type)));
    }
    connect(elementButton, &QPushButton::clicked, this, [&](bool checked) {
      setElementSelected(element.name, checked);
      updateValue();
    });
    periodicTableLayout->addWidget(elementButton, element.yPos + 1, element.xPos);
    elementButtons[element.name] = elementButton;
  }

  // create a subwidget for the list of currently selected elements, including a label and clear button
  auto *valueDisplayWidget = new QWidget(getWidget());
  valueDisplayWidget->setMaximumWidth(periodicTableWidget->width());
  auto *valueDisplayWidgetLayout = new QVBoxLayout();
  valueDisplayWidget->setLayout(valueDisplayWidgetLayout);
  valueText = new QTextEdit(getWidget());
  valueText->setAlignment(Qt::AlignLeft);
  valueText->setAutoFillBackground(false);
  valueText->setReadOnly(true);
  valueText->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Preferred);
  valueText->setFixedHeight(TEXTEDIT_HEIGHT); // enough space to display all elements without scrollbars
  valueText->setStyleSheet("QTextEdit { background-color: #ececec; }"); // same background color as the panel
  auto *clearButton = new QPushButton("clear");
  clearButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
  connect(clearButton, &QPushButton::clicked, this, &PeriodicTableWidget::clearSelection);
  auto *infoLineWidget = new QWidget(getWidget());
  auto *infoLineLayout = new QHBoxLayout(getWidget());
  infoLineWidget->setLayout(infoLineLayout);
  auto *descriptionLabel = new QLabel("Selected elements:");
  infoLineLayout->addWidget(descriptionLabel);
  infoLineLayout->addWidget(clearButton);
  valueDisplayWidgetLayout->addWidget(infoLineWidget);
  valueDisplayWidgetLayout->addWidget(valueText);

  auto *layout = new QVBoxLayout();
  getWidget()->setLayout(layout);
  layout->addWidget(periodicTableWidget);
  layout->addWidget(valueDisplayWidget);
}

void PeriodicTableWidget::synchronizeVTI() {
  Ambassador *innerAmbassador = getInnerAmbassador();
  std::vector<std::string> value;
  innerAmbassador->getValue("selectedElements", value);
  setValue(value);
}

void PeriodicTableWidget::updateValue() {
  std::vector<std::string> value;
  std::string stringRepresentation;
  if (selectedElements.empty()) {
    stringRepresentation = "None selected";
  } else {
    for (const std::string &str : selectedElements) {
      value.push_back(str);
      if (!stringRepresentation.empty()) {
        stringRepresentation += ", ";
      }
      stringRepresentation += str;
    }
  }
  getInnerAmbassador()->setValue("selectedElements", value);
}

void PeriodicTableWidget::setElementSelected(const std::string& elementName, bool selected) {
  if (selected) {
    selectedElements.insert(elementName);
  } else {
    selectedElements.erase(elementName);
  }
}

void PeriodicTableWidget::clearSelection() {
  for (const std::string &str : selectedElements) {
    setElementButtonState(str, false);
  }
  selectedElements.clear();
  updateValue();
}

void PeriodicTableWidget::setElementButtonState(const std::string &elementName, bool checked) {
  auto element = elementButtons.find(elementName);
  if (element != elementButtons.end()) {
    QPushButton *elementButton = element->second;
    if (elementButton) {
      elementButton->setChecked(checked);
    }
  }
}

void PeriodicTableWidget::setValue(const std::vector<std::string>& selectedElements) {
  std::string stringRepresentation;
  for (auto &element : elementButtons) {
    QPushButton *elementButton = element.second;
    elementButton->setChecked(false);
  }
  if (selectedElements.empty()) {
    stringRepresentation = "None selected";
  } else {
    for (const std::string &selectedElement : selectedElements) {
      if (selectedElement.empty()) continue;
      if (!stringRepresentation.empty()) {
        stringRepresentation += ", ";
      }
      stringRepresentation += selectedElement;

      setElementSelected(selectedElement, true);
      setElementButtonState(selectedElement, true);
    }
  }
  valueText->setText(QString::fromStdString(stringRepresentation));
}

const int PeriodicTableWidget::ELEMENT_SIZE = 30;
const int PeriodicTableWidget::TEXTEDIT_HEIGHT = 90;

// periodic table data was obtained and parsed from
// https://raw.githubusercontent.com/Bowserinator/Periodic-Table-JSON/master/PeriodicTableJSON.json
const std::vector<Element> PeriodicTableWidget::elements  = {
  {"H", DIATOMIC_NONMETAL, 1, 1},
  {"He", NOBLE_GAS, 18, 1},
  {"Li", ALKALI_METAL, 1, 2},
  {"Be", ALKALINE_EARTH_METAL, 2, 2},
  {"B", METALLOID, 13, 2},
  {"C", POLYATOMIC_NONMETAL, 14, 2},
  {"N", DIATOMIC_NONMETAL, 15, 2},
  {"O", DIATOMIC_NONMETAL, 16, 2},
  {"F", DIATOMIC_NONMETAL, 17, 2},
  {"Ne", NOBLE_GAS, 18, 2},
  {"Na", ALKALI_METAL, 1, 3},
  {"Mg", ALKALINE_EARTH_METAL, 2, 3},
  {"Al", POST_TRANSITION_METAL, 13, 3},
  {"Si", METALLOID, 14, 3},
  {"P", POLYATOMIC_NONMETAL, 15, 3},
  {"S", POLYATOMIC_NONMETAL, 16, 3},
  {"Cl", DIATOMIC_NONMETAL, 17, 3},
  {"Ar", NOBLE_GAS, 18, 3},
  {"K", ALKALI_METAL, 1, 4},
  {"Ca", ALKALINE_EARTH_METAL, 2, 4},
  {"Sc", TRANSITION_METAL, 3, 4},
  {"Ti", TRANSITION_METAL, 4, 4},
  {"V", TRANSITION_METAL, 5, 4},
  {"Cr", TRANSITION_METAL, 6, 4},
  {"Mn", TRANSITION_METAL, 7, 4},
  {"Fe", TRANSITION_METAL, 8, 4},
  {"Co", TRANSITION_METAL, 9, 4},
  {"Ni", TRANSITION_METAL, 10, 4},
  {"Cu", TRANSITION_METAL, 11, 4},
  {"Zn", TRANSITION_METAL, 12, 4},
  {"Ga", POST_TRANSITION_METAL, 13, 4},
  {"Ge", METALLOID, 14, 4},
  {"As", METALLOID, 15, 4},
  {"Se", POLYATOMIC_NONMETAL, 16, 4},
  {"Br", DIATOMIC_NONMETAL, 17, 4},
  {"Kr", NOBLE_GAS, 18, 4},
  {"Rb", ALKALI_METAL, 1, 5},
  {"Sr", ALKALINE_EARTH_METAL, 2, 5},
  {"Y", TRANSITION_METAL, 3, 5},
  {"Zr", TRANSITION_METAL, 4, 5},
  {"Nb", TRANSITION_METAL, 5, 5},
  {"Mo", TRANSITION_METAL, 6, 5},
  {"Tc", TRANSITION_METAL, 7, 5},
  {"Ru", TRANSITION_METAL, 8, 5},
  {"Rh", TRANSITION_METAL, 9, 5},
  {"Pd", TRANSITION_METAL, 10, 5},
  {"Ag", TRANSITION_METAL, 11, 5},
  {"Cd", TRANSITION_METAL, 12, 5},
  {"In", POST_TRANSITION_METAL, 13, 5},
  {"Sn", POST_TRANSITION_METAL, 14, 5},
  {"Sb", METALLOID, 15, 5},
  {"Te", METALLOID, 16, 5},
  {"I", DIATOMIC_NONMETAL, 17, 5},
  {"Xe", NOBLE_GAS, 18, 5},
  {"Cs", ALKALI_METAL, 1, 6},
  {"Ba", ALKALINE_EARTH_METAL, 2, 6},
  {"La", LANTHANIDE, 3, 9},
  {"Ce", LANTHANIDE, 4, 9},
  {"Pr", LANTHANIDE, 5, 9},
  {"Nd", LANTHANIDE, 6, 9},
  {"Pm", LANTHANIDE, 7, 9},
  {"Sm", LANTHANIDE, 8, 9},
  {"Eu", LANTHANIDE, 9, 9},
  {"Gd", LANTHANIDE, 10, 9},
  {"Tb", LANTHANIDE, 11, 9},
  {"Dy", LANTHANIDE, 12, 9},
  {"Ho", LANTHANIDE, 13, 9},
  {"Er", LANTHANIDE, 14, 9},
  {"Tm", LANTHANIDE, 15, 9},
  {"Yb", LANTHANIDE, 16, 9},
  {"Lu", LANTHANIDE, 17, 9},
  {"Hf", TRANSITION_METAL, 4, 6},
  {"Ta", TRANSITION_METAL, 5, 6},
  {"W", TRANSITION_METAL, 6, 6},
  {"Re", TRANSITION_METAL, 7, 6},
  {"Os", TRANSITION_METAL, 8, 6},
  {"Ir", TRANSITION_METAL, 9, 6},
  {"Pt", TRANSITION_METAL, 10, 6},
  {"Au", TRANSITION_METAL, 11, 6},
  {"Hg", TRANSITION_METAL, 12, 6},
  {"Tl", POST_TRANSITION_METAL, 13, 6},
  {"Pb", POST_TRANSITION_METAL, 14, 6},
  {"Bi", POST_TRANSITION_METAL, 15, 6},
  {"Po", POST_TRANSITION_METAL, 16, 6},
  {"At", METALLOID, 17, 6},
  {"Rn", NOBLE_GAS, 18, 6},
  {"Fr", ALKALI_METAL, 1, 7},
  {"Ra", ALKALINE_EARTH_METAL, 2, 7},
  {"Ac", ACTINIDE, 3, 10},
  {"Th", ACTINIDE, 4, 10},
  {"Pa", ACTINIDE, 5, 10},
  {"U", ACTINIDE, 6, 10},
  {"Np", ACTINIDE, 7, 10},
  {"Pu", ACTINIDE, 8, 10},
  {"Am", ACTINIDE, 9, 10},
  {"Cm", ACTINIDE, 10, 10},
  {"Bk", ACTINIDE, 11, 10},
  {"Cf", ACTINIDE, 12, 10},
  {"Es", ACTINIDE, 13, 10},
  {"Fm", ACTINIDE, 14, 10},
  {"Md", ACTINIDE, 15, 10},
  {"No", ACTINIDE, 16, 10},
  {"Lr", ACTINIDE, 17, 10},
  {"Rf", TRANSITION_METAL, 4, 7},
  {"Db", TRANSITION_METAL, 5, 7},
  {"Sg", TRANSITION_METAL, 6, 7},
  {"Bh", TRANSITION_METAL, 7, 7},
  {"Hs", TRANSITION_METAL, 8, 7},
  {"Mt", TRANSITION_METAL, 9, 7},
  {"Ds", TRANSITION_METAL, 10, 7},
  {"Rg", TRANSITION_METAL, 11, 7},
  {"Cn", TRANSITION_METAL, 12, 7},
  {"Nh", TRANSITION_METAL, 13, 7},
  {"Fl", POST_TRANSITION_METAL, 14, 7},
  {"Mc", POST_TRANSITION_METAL, 15, 7},
  {"Lv", POST_TRANSITION_METAL, 16, 7},
  {"Ts", METALLOID, 17, 7},
  {"Og", NOBLE_GAS, 18, 7},
  {"Uue", ALKALI_METAL, 1, 8}
};

const std::unordered_map<ElementType, QString> PeriodicTableWidget::elementTypeColors = {
  {ACTINIDE, QString("6ba54b")},
  {ALKALI_METAL, QString("d9514c")},
  {ALKALINE_EARTH_METAL,QString("c3c3c3")},
  {DIATOMIC_NONMETAL, QString("f2b01d")},
  {LANTHANIDE, QString("543210")},
  {METALLOID, QString("123456")},
  {NOBLE_GAS, QString("a0b3cf")},
  {POLYATOMIC_NONMETAL, QString("3b91ae")},
  {POST_TRANSITION_METAL, QString("ae4747")},
  {TRANSITION_METAL, QString("eedb4f")},
  {UNKNOWN, QString("999999")}
};
