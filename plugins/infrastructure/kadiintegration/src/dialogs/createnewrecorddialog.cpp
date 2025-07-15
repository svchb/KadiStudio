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

#include <iostream>
#include <utility>
#include <memory>

#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>

#include <QVector>
#include <QFormLayout>
#include <QCheckBox>
#include <QComboBox>
#include <QLineEdit>
#include <QDialogButtonBox>
#include <QMessageBox>

#include <QNetworkAccessManager>
#include <QUrlQuery>
#include <QUrl>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QProcess>
#include <QToolTip>
#include <QMovie>
#include <QLabel>

#include "../domain/templateinfo.h"
#include "../utils/kadiutils.h"

#include "createnewrecorddialog.h"

CreateNewRecordDialog::CreateNewRecordDialog(KadiInstance instance) : networkAccessManager(new QNetworkAccessManager(this)), kadiInstance(std::move(instance)), allTemplates(new QVector<TemplateInfo>(8)) {
  setWindowTitle(tr("Create A New Record In Kadi"));
  QFormLayout *layout = new QFormLayout(this);

  useTemplateCheckbox = new QCheckBox(tr("Use Template:"));

  templateComboBox = new QComboBox();
  templateComboBox->setMinimumWidth(250);
  templateComboBox->setDisabled(true);

  recordIdentifierField = new QLineEdit();
  recordIdentifierField->setMinimumWidth(250);

  recordTitleField = new QLineEdit();
  recordTitleField->setMinimumWidth(250);

  loadingIndicatorLbl = new QLabel(tr("loading ..."));
  loadingGif = new QMovie(":/studio/plugins/infrastructure/kadiintegration/img/loading.gif");
  loadingIndicatorLbl->setMovie(loadingGif);
  loadingGif->start();
  hideLoadingIndicator();

  buttonBox = new QDialogButtonBox();
  buttonBox->addButton(tr("Create"), QDialogButtonBox::AcceptRole);
  buttonBox->addButton(QDialogButtonBox::Cancel);
  buttonBox->setDisabled(true);

  layout->addRow(useTemplateCheckbox, templateComboBox);
  layout->addRow(tr("Record Identifier:"), recordIdentifierField);
  layout->addRow(tr("Record Title:"), recordTitleField);
  layout->addRow(loadingIndicatorLbl, buttonBox);

  connect(useTemplateCheckbox, &QCheckBox::toggled, templateComboBox, &QComboBox::setEnabled);
  connect(useTemplateCheckbox, &QCheckBox::toggled, this, [=, this]() { this->templateSelectionChanged(templateComboBox->currentIndex()); });
  connect(templateComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &CreateNewRecordDialog::templateSelectionChanged);
  connect(buttonBox, &QDialogButtonBox::accepted, this, &CreateNewRecordDialog::accept);
  connect(buttonBox, &QDialogButtonBox::rejected, this, &CreateNewRecordDialog::reject);
}

int CreateNewRecordDialog::exec() {
  // fetch templates from kadi instance
  allTemplates->clear();
  fetchTemplates(1);

  return QDialog::exec();
}

void CreateNewRecordDialog::accept() {
  buttonBox->setDisabled(true);
  showLoadingIndicator();
  kadiutils::yieldForMilliseconds(20);

  // check inputs
  if (recordIdentifierField->text().isEmpty()) {
    QToolTip::showText(this->pos() + buttonBox->pos(), tr("The record identifier must not be empty"), buttonBox);
    buttonBox->setDisabled(false);
    hideLoadingIndicator();
    return;
  }

  // create record
  if (!createRecord()) {
    buttonBox->setDisabled(false);
    hideLoadingIndicator();
    return; // record creation failed, msgbox shown in createRecord()
  }

  // add metadata from template
  if (useTemplateCheckbox->isChecked() && !addMetadataFromTemplate()) {
    buttonBox->setDisabled(false);
    hideLoadingIndicator();
    return; // adding metadata failed, msgbox shown in createRecord()
  }

  QMessageBox::information(this, tr("Record creation"), tr("The record was successfully created."));

  buttonBox->setDisabled(false);
  hideLoadingIndicator();
  QDialog::accept();
}

QString CreateNewRecordDialog::getCreatedRecordIdentifier() {
  return recordIdentifierField->text().trimmed();
}

void CreateNewRecordDialog::templateSelectionChanged(int index) {
  if (index > -1 && useTemplateCheckbox->isChecked()) {
    auto selectedTemplate = allTemplates->at(index);
    if (selectedTemplate.type == "record") {
      if (!selectedTemplate.recordInfo.identifier.isEmpty()) {
        recordIdentifierField->setText(selectedTemplate.recordInfo.identifier);
      }
      if (!selectedTemplate.recordInfo.title.isEmpty()) {
        recordTitleField->setText(selectedTemplate.recordInfo.title);
      }
    }
  }
}

void CreateNewRecordDialog::fetchTemplates(int pageNumber) {
  // build the api/template url with 100 results per page
  if (!kadiInstance.host.endsWith('/')) {
    kadiInstance.host.append('/');
  }
  QUrl url(kadiInstance.host + "api/templates");
  QUrlQuery query(url);
  query.addQueryItem("page", QString::number(pageNumber));
  query.addQueryItem("per_page", "100");
  url.setQuery(query);
  QNetworkRequest request(url);

  request.setRawHeader("Authorization", (QString("Bearer ") + kadiInstance.token).toUtf8());

  auto requesthandler = new kadiutils::NetworkHandler(networkAccessManager);
  connect(requesthandler, &kadiutils::NetworkHandler::finished, this, [this, requesthandler](QNetworkReply* reply) {
    processTemplatesReply(reply);
    delete requesthandler;
  });
  requesthandler->sendRequest(request);
}

void CreateNewRecordDialog::processTemplatesReply(QNetworkReply *reply) {
  int statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();

  // check if request was successful
  if (statusCode < 200 || statusCode > 299) {
    kadiutils::reportErrorToUser(reply, this);
    return;
  }

  // parse the json in the reply body
  QJsonObject jsonobject;
  if (not kadiutils::parseJsonData(reply, jsonobject)) {
    return;
  }

  for (auto value : jsonobject["items"].toArray()) {
    QJsonObject item = value.toObject();
    TemplateInfo templateInfo = {
      .id = item["id"].toInt(),
      .identifier = item["identifier"].toString(),
      .type = item["type"].toString(),
      .title = item["title"].toString(),
      .description = item["description"].toString(),
      .metadata = item["data"].toString(),
      .extrametadata = "",
      .recordInfo = RecordInfo{}
    };

    if (templateInfo.type == "record") {
      // in this case the type of the template is "record", which means that the template also includes title, desc, etc. for the record which will be created
      QJsonObject itemdata = item["data"].toObject();
      templateInfo.recordInfo.identifier = itemdata["identifier"].toString();
      templateInfo.recordInfo.title = itemdata["title"].toString();
      templateInfo.recordInfo.description = itemdata["description"].toString();
      if (itemdata["type"].isString()) {
        templateInfo.recordInfo.type = itemdata["type"].toString();
      } else {
        templateInfo.recordInfo.type = "";
      }

      if (itemdata["tags"].isArray()) {
        for (auto tag: itemdata["tags"].toArray()) {
          templateInfo.recordInfo.tags.append(tag.toString());
        }
      }

      if (itemdata["license"].isString()) {
        templateInfo.recordInfo.license = itemdata["license"].toString();
      }

      templateInfo.extrametadata = itemdata["extras"].toString();
    } else {
      templateInfo.extrametadata = templateInfo.metadata;
    }

    allTemplates->append(templateInfo);
    templateComboBox->addItem(templateInfo.identifier); // add the item to the combobox here
  }

  // check pagination info if more templates are available
  if (jsonobject["_pagination"].toObject()["_links"].toObject()["next"].isString()) {
    auto ogRequest = reply->request();
    QUrlQuery ogQuery(ogRequest.url());
    int currentPage = ogQuery.queryItemValue("page").toInt();
    fetchTemplates(currentPage + 1);
  } else {
    if (useTemplateCheckbox->isChecked()) {
      templateComboBox->setDisabled(false);
    }
    buttonBox->setDisabled(false);
  }
}

bool CreateNewRecordDialog::createRecord() {
  QProcess proc;

  // try to create the record
  proc.start("kadi-apy", {
    "records",
    "create",
    "-I", kadiInstance.name,
    "-i", recordIdentifierField->text().trimmed(),
    "-t", recordTitleField->text().trimmed(),
    "-e"  // return error if the record is not created
  });
  proc.waitForFinished();

  if (proc.exitCode() != 0) {
    kadiutils::showWarningWithProcOutput(this, tr("Record Creation from Template"), tr("Record creation failed. Output from kadi-apy:"), proc);
    return false;
  }

  return true;
}

bool CreateNewRecordDialog::addMetadataFromTemplate() {
  // No temlate is selected or templates list is empty
  if (templateComboBox->currentIndex() == -1 || allTemplates->isEmpty()) return true;

  TemplateInfo selectedTemplate = allTemplates->at(templateComboBox->currentIndex());
  QString chosenIdentifier = recordIdentifierField->text().trimmed();

  QProcess proc;

  // try to add metadata to the record
  proc.start("kadi-apy", {
    "records",
    "add-metadata",
    "-I", kadiInstance.name,
    "-R", chosenIdentifier,
    "-m", selectedTemplate.extrametadata,
    "-f", // force overwrite all existing (should not be necessary normally)
  });
  proc.waitForFinished();

  if (proc.exitCode() != 0) {
    kadiutils::showWarningWithProcOutput(this, tr("Adding Metadata to Record"), tr("Adding metadata to record failed. Output from kadi-apy:"), proc);
    return false;
  }

  // add description, type and license
  QStringList args = {
      "records",
      "edit",
      "-I", kadiInstance.name,
      "-R", chosenIdentifier,
      "--description", selectedTemplate.recordInfo.description,
  };
  if (!selectedTemplate.recordInfo.type.isEmpty()) {
    args.append({ "--type", selectedTemplate.recordInfo.type });
  }
  if (!selectedTemplate.recordInfo.license.isEmpty()) {
    args.append({ "--license", selectedTemplate.recordInfo.license });
  }

  proc.start("kadi-apy", args);
  proc.waitForFinished();

  if (proc.exitCode() != 0) {
    kadiutils::showWarningWithProcOutput(this, tr("Adding Metadata to Record"), tr("Adding description, type and license to record failed. Output from kadi-apy:"), proc);
    return false;
  }

  // add tags
  for (const QString &tag : selectedTemplate.recordInfo.tags) {
    proc.start("kadi-apy", {
      "records",
      "add-tag",
      "-I", kadiInstance.name,
      "-R", chosenIdentifier,
      "--tag", tag,
    });
    proc.waitForFinished();

    if (proc.exitCode() != 0) {
      kadiutils::showWarningWithProcOutput(this, tr("Adding Metadata to Record"), tr("Adding a tag to record failed. Output from kadi-apy:"), proc);
      return false;
    }
  }

  return true;
}

void CreateNewRecordDialog::showLoadingIndicator() {
  loadingIndicatorLbl->setDisabled(false);
  loadingIndicatorLbl->setMovie(loadingGif);
  loadingGif->setPaused(false);
}

void CreateNewRecordDialog::hideLoadingIndicator() {
  loadingGif->setPaused(true);
  loadingIndicatorLbl->setMovie(nullptr);
  loadingIndicatorLbl->setDisabled(true);
}
