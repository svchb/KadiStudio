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

#include <set>
#include <functional>
#include <QDialog>
#include <QJsonArray>
#include <utility>
#include "../editvariablesdialoginterface.h"

class QTableWidget;
class QLabel;

/**
 * @brief      A dialog to show and edit variables
 * @ingroup    src
 */
class EditVariablesDialog : public QDialog, public EditVariablesDialogInterface {
    Q_OBJECT

  public:
    EditVariablesDialog();

    void showEditVariablesDialog(const QJsonArray& variables, std::function<void(QJsonArray)> on_success,
                                 std::function<bool(const QJsonArray&)> has_changed) override;

  signals:
    void errorsChanged(const QString& errortext);

  private slots:
    void addEmptyRow();
    void removeSelectedRows();
    void validate();
    void onSuccess(QJsonArray result) {
      on_success(std::move(result));
    }

  private:
    QColor getColorForRow(int row); // only for BackgroundColorDelegate
    QJsonArray getVariables();
    void addError(const QString& error);
    void clearErrors();
    QString getVariableName(int row);
    QString getVariableValue(int row);
    void loadVariables(const QJsonArray& variables);
    void addVariable(const QString& name = "", const QString& value = "");
    void initGuiElements();
    QString errorText();

    std::function<void(QJsonArray)> on_success;
    std::function<bool(const QJsonArray&)> has_changed;
    QTableWidget *variables_table;

    // for validation
    std::set<int> rows_with_duplicates;
    std::set<int> rows_with_errors;
    QLabel *errors_label;
    std::set<QString> errors;

    friend class BackgroundColorDelegate;
};
