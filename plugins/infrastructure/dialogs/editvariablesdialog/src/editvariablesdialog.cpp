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

#include "editvariablesdialog.h"
#include "backgroundcolordelegate.h"

#include <unordered_map>
#include <QVBoxLayout>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QTableWidget>
#include <QHeaderView>
#include <QModelIndex>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>
#include <QLabel>
#include <QMessageBox>

EditVariablesDialog::EditVariablesDialog()
    : variables_table(new QTableWidget(0, 2)), errors_label(new QLabel()) {
  setModal(true);
  setMinimumWidth(450);
  initGuiElements();
}

QJsonArray EditVariablesDialog::getVariables() {
  QJsonArray result;
  for (int row = 0; row < variables_table->rowCount(); row++) {
    QModelIndex name_index = variables_table->model()->index(row, 0);
    QString variable_name = variables_table->model()->data(name_index).toString();
    QModelIndex value_index = variables_table->model()->index(row, 1);
    QString variable_value = variables_table->model()->data(value_index).toString();
    if (variable_name.isEmpty() && variable_value.isEmpty()) {
      continue;
    }
    result.push_back(QJsonObject {{"name", variable_name}, {"value", variable_value}});
  }
  return result;
}

void EditVariablesDialog::loadVariables(const QJsonArray &variables) {
  variables_table->setRowCount(0); // to clear all entries
  for (const auto& variable : variables) {
    QJsonObject variable_object = variable.toObject();
    if (variable_object.contains("name") && variable_object.contains("value")) {
      QString variable_name = variable_object["name"].toString();
      QString variable_value = variable_object["value"].toString();
      addVariable(variable_name, variable_value);
    }
  }
}

void EditVariablesDialog::addVariable(const QString &name, const QString &value) {
  int row = variables_table->rowCount();
  variables_table->insertRow(row);
  QModelIndex name_index = variables_table->model()->index(row, 0);
  variables_table->model()->setData(name_index, name);
  QModelIndex value_index = variables_table->model()->index(row, 1);
  variables_table->model()->setData(value_index, value);
}

void EditVariablesDialog::addEmptyRow() {
  addVariable();
}

void EditVariablesDialog::removeSelectedRows() {
  QList<QTableWidgetItem*> selected_items = variables_table->selectedItems();
  QList<QPersistentModelIndex> indices_to_delete;

  // create persistent indices first, otherwise the application can crash depending on the order of deletion
  for (QTableWidgetItem* selected_item : selected_items) {
    indices_to_delete.push_back(QPersistentModelIndex(variables_table->model()->index(selected_item->row(), 0)));
  }
  // now the rows which are referenced by the persistent indices can be safely deleted
  for (const QPersistentModelIndex& selected_index : indices_to_delete) {
    int row = selected_index.row();
    if (row >= 0 && row < variables_table->rowCount()) {
      variables_table->removeRow(row);
    }
  }
}

void EditVariablesDialog::initGuiElements() {
  setWindowTitle(tr("Edit Workflow Variables"));
  auto dialog_layout = new QVBoxLayout();
  connect(this, &QDialog::accepted, this, [this]() {
    onSuccess(getVariables());
  });

  variables_table->horizontalHeader()->setStretchLastSection(true);
  variables_table->setHorizontalHeaderLabels({tr("Name"), tr("Value")});
  variables_table->setSelectionBehavior(QAbstractItemView::SelectRows);
  variables_table->setItemDelegate(new BackgroundColorDelegate(this));
  connect(variables_table->model(), &QAbstractTableModel::dataChanged, this, &EditVariablesDialog::validate); // not triggered when rows are removed!
  connect(variables_table->model(), &QAbstractTableModel::rowsRemoved, this, &EditVariablesDialog::validate);
  dialog_layout->addWidget(variables_table);

  errors_label->setStyleSheet("color: red;");
  connect(this, &EditVariablesDialog::errorsChanged, errors_label, &QLabel::setText);
  dialog_layout->addWidget(errors_label);

  QDialogButtonBox *button_box = new QDialogButtonBox();
  auto add_variable_button = button_box->addButton(tr("Add Variable"), QDialogButtonBox::ActionRole);
  connect(add_variable_button, &QPushButton::clicked, this, &EditVariablesDialog::addEmptyRow);
  auto delete_variable_button = button_box->addButton(tr("Delete Variable"), QDialogButtonBox::ActionRole);
  connect(delete_variable_button, &QPushButton::clicked, this, &EditVariablesDialog::removeSelectedRows);
  connect(variables_table, &QTableWidget::itemSelectionChanged, this, [delete_variable_button, this]() {
    delete_variable_button->setDisabled(variables_table->selectedItems().empty());
  });
  auto apply_button = button_box->addButton(QDialogButtonBox::Ok);//new QPushButton("Apply");
  connect(apply_button, &QPushButton::clicked, this, &EditVariablesDialog::accept);
  connect(this, &EditVariablesDialog::errorsChanged, this, [apply_button] (const QString& errortext) {
    apply_button->setEnabled(errortext.isEmpty());
  });
  auto cancel_button = button_box->addButton(QDialogButtonBox::Cancel);//new QPushButton("Cancel");
  connect(cancel_button, &QPushButton::clicked, this, [this]() {
    if (has_changed(getVariables())) {
      auto reply = QMessageBox::question(this, tr("Cancel editing?"), tr("Are you sure you want to discard unsaved changes?"), QMessageBox::Yes | QMessageBox::No);
      if (reply == QMessageBox::Yes) {
        reject();
      }
    } else {
      reject();
    }
  });

  dialog_layout->addWidget(button_box);

  setLayout(dialog_layout);
}

void EditVariablesDialog::showEditVariablesDialog(const QJsonArray& variables, std::function<void(QJsonArray)> f_on_success,
                                                  std::function<bool(const QJsonArray &)> f_has_changed) {
  on_success = f_on_success;
  has_changed = f_has_changed;
  loadVariables(variables);
  show();
}

QColor EditVariablesDialog::getColorForRow(int row) {
  if (rows_with_errors.find(row) != rows_with_errors.end()) {
    return Qt::red;
  } else if (rows_with_duplicates.find(row) != rows_with_duplicates.end()) {
    return Qt::yellow;
  } else {
    return Qt::white;
  }
}

QString EditVariablesDialog::getVariableName(int row) {
  QModelIndex name_index = variables_table->model()->index(row, 0);
  return variables_table->model()->data(name_index).toString();
}

QString EditVariablesDialog::getVariableValue(int row) {
  QModelIndex index = variables_table->model()->index(row, 1);
  return variables_table->model()->data(index).toString();
}

void EditVariablesDialog::validate() {
  std::set<int> duplicate_rows;
  std::set<int> error_rows;
  std::unordered_map<std::string, int> unique_names;
  clearErrors();
  for (int row_index = 0; row_index < variables_table->rowCount(); ++row_index) {
    QString name = getVariableName(row_index);
    QString value = getVariableValue(row_index);
    if (name.isEmpty()) {
      if (!value.isEmpty()) {
        error_rows.insert(row_index);
        addError(tr("Variables names must not be empty"));
      }
      // if both name and value is empty, ignore this entry entirely (will not be included in json as well)
      continue;
    }
    const auto& entry = unique_names.find(name.toStdString());
    if (entry != unique_names.end()) {
      addError(tr("Variable names must be unique"));
      duplicate_rows.insert(row_index);
      duplicate_rows.insert((*entry).second); // also mark the row index of the first occurrence
    } else {
      unique_names[name.toStdString()] = row_index;
    }
  }
  rows_with_duplicates = duplicate_rows;
  rows_with_errors = error_rows;
}

void EditVariablesDialog::addError(const QString& error) {
  errors.insert(error);
  emit errorsChanged(errorText());
}

void EditVariablesDialog::clearErrors() {
  errors.clear();
  emit errorsChanged("");
}

QString EditVariablesDialog::errorText() {
  QString result;
  for (const auto& error : errors) {
    result.append(error);
    result.append("\n");
  }
  return result;
}
