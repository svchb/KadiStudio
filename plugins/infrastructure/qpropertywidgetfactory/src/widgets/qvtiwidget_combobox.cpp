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

#include <QHBoxLayout>

#include "qvtiwidget_combobox.h"

QVTIWidget_combobox::QVTIWidget_combobox(AbstractValueTypeInterface* valuetypeinterface, QWidget* parent)
    : QVTIWidget(valuetypeinterface, new QComboBox(parent)) {

  connect(getComboBox(), &QComboBox::currentIndexChanged, this, [&]([[maybe_unused]]int index) {
    setValue(getComboBox()->currentData(Qt::UserRole).toString().toStdString());
  });
}

void QVTIWidget_combobox::synchronizeVTI() {
  const std::vector<std::string>& options = valuetypeinterface->getHint()->getValidator<ListValidator<std::string>>()->getList();

  getComboBox()->blockSignals(true); // otherwise, addItem() will emit currentIndexChanged, which will trigger this method again via entrySelected()
  getComboBox()->clear();
  for (const std::string& option : options) {
    size_t found = option.find("@");
    if (found == std::string::npos) {
      getComboBox()->addItem(QString::fromStdString(option), QString::fromStdString(option));
    } else {
      std::string description = option;
      description.replace(found, 1, " : ");
      getComboBox()->addItem(QString::fromStdString(description), QString::fromStdString(option.substr(0, found)));
    }
  }

  // set previously selected value (restore value from property)
  const std::string &selectedEntry = getValue<std::string>();

  int index = getComboBox()->findData(QVariant(selectedEntry.c_str()), Qt::UserRole, Qt::MatchExactly);

  if (index != -1) {
    getComboBox()->setCurrentIndex(index);
  } else if (options.size() > 0) {
    // otherwise, it's impossible to choose from a list with only one item (for example)
    getComboBox()->setCurrentIndex(0);
    // signals may be processed delayed, so we need to unblock them after all items were added,
    // see https://forum.qt.io/topic/131528/qdoublespinbox-blocksignals-does-not-work/4
    // but seems to work with explicit setValue()
    setValue(getComboBox()->currentData(Qt::UserRole).toString().toStdString());
  } else {
    getComboBox()->setEnabled(false);
  }
  getComboBox()->blockSignals(false);
}
