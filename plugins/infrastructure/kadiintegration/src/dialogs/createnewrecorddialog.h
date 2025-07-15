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

#include <QDialog>
#include <plugins/infrastructure/kadiconfig/kadiinstance.h>

// forward declarations
struct TemplateInfo;

class QCheckBox;
class QComboBox;
class QDialogButtonBox;
class QLineEdit;
class QPushButton;
class QString;
class QNetworkAccessManager;
class QNetworkReply;
class QMovie;
class QLabel;


/**
 * @brief      Show the dialog in which the user is able to create a new record in Kadi.
 * @ingroup    kadiintegration
 */
class CreateNewRecordDialog : public QDialog {
  Q_OBJECT

  public:
    explicit CreateNewRecordDialog(KadiInstance instance);

    int exec() override;

    void accept() override;

    QString getCreatedRecordIdentifier();

  private:
    QCheckBox *useTemplateCheckbox;
    QComboBox *templateComboBox;
    QLineEdit *recordIdentifierField;
    QLineEdit *recordTitleField;
    QDialogButtonBox *buttonBox;

    QMovie *loadingGif;
    QLabel *loadingIndicatorLbl;

    QNetworkAccessManager *networkAccessManager;

    KadiInstance kadiInstance;

    QVector<TemplateInfo> *allTemplates = nullptr;

    void fetchTemplates(int pageNumber);

    void processTemplatesReply(QNetworkReply *reply);

    bool createRecord();

    bool addMetadataFromTemplate();

    void templateSelectionChanged(int index);

    void showLoadingIndicator();

    void hideLoadingIndicator();
};
