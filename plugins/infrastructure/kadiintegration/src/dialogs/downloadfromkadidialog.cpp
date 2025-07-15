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

#include <cmath>

#include <QString>
#include <QAtomicInt>
#include <QJsonArray>
#include <QJsonValue>

#include <QMimeDatabase>
#include <QMimeData>
#include <QListView>
#include <QSortFilterProxyModel>
#include <QCompleter>
#include <QFormLayout>
#include <QComboBox>
#include <QIcon>
#include <QLabel>
#include <QLineEdit>
#include <QTreeWidget>
#include <QPushButton>
#include <QMovie>
#include <QMessageBox>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QUrlQuery>

#include <QFile>
#include <QDir>
#include <QTemporaryDir>

#include <framework/enhanced/qlineeditclearable.h>
#include <framework/pluginframework/pluginmanagerinterface.h>

#include <plugins/infrastructure/kadiconfig/kadiconfiginterface.h>

#include "../utils/kadiutils.h"
#include "../kadiintegration.h"
#include "../domain/recordinfo.h"
#include "../domain/recordfileinfo.h"
#include "createnewrecorddialog.h"
#include "plugins/infrastructure/kadiintegration/src/dialogs/uploadtokadidialog.h"

#include "downloadfromkadidialog.h"


#define KADI_RECORDS_ENDPOINT "api/records"

static const char *workflowmimetype = "application/x-flow+json";

DownloadFromKadiDialog::DownloadFromKadiDialog(LibFramework::PluginManagerInterface* pluginmanager, QWidget* parent)
    : QDialog(parent), pluginmanager(pluginmanager), loadedRecords(0), totalItems(0) {

  networkAccessManager = new QNetworkAccessManager(this);
  activeRequests.storeRelease(0);

  pageSize = new QComboBox(this);
  pageSize->addItems({ "10", "20", "30", "40", "50", "60", "70", "80", "90", "100" });
  pageSize->setToolTip(tr("Amount of items retrieved per request"));

  filterField = new QLineEditClearable(this);
  filterField->setPlaceholderText(tr("Enter filter string"));
  filterField->setToolTip(tr("The API uses fuzzy matching. Exact matches can be requested by using double quotes, e.g. \"your-query\"."));
  filterField->setMinimumWidth(400);

  mimeTypeFilter = new QLineEditClearable(this);
  mimeTypeFilter->setToolTip(tr("Filter query by this mimetype."));
  QMimeDatabase db;
  QStringList mimeypelist;
  for (auto mimetype : db.allMimeTypes()) {
    mimeypelist << mimetype.name();
  }
  mimeypelist << workflowmimetype;
  mimeypelist.sort(Qt::CaseInsensitive);
  QCompleter *mimetypecompleter = new QCompleter(mimeypelist, this);
  mimetypecompleter->setCompletionMode(QCompleter::PopupCompletion);
  mimetypecompleter->setFilterMode(Qt::MatchContains);
  mimetypecompleter->setWrapAround(true);
  mimetypecompleter->setModelSorting(QCompleter::CaseInsensitivelySortedModel);
  mimetypecompleter->popup()->setFocusPolicy(Qt::NoFocus);
  mimeTypeFilter->setCompleter(mimetypecompleter);

  fetchRecordsBtn = new QPushButton(tr("Fetch Records"), this);
  loadMoreRecordsBtn = new QPushButton(tr("Load More"));
  loadMoreRecordsBtn->setDisabled(true);

  treeWidget = new QTreeWidget(this);
  treeWidget->setColumnCount(2);
  treeWidget->setHeaderLabels({ tr("Record Identifier / File Name"), tr("Last Modified") });
  treeWidget->setColumnWidth(0, 400);
  treeWidget->setSortingEnabled(true);
  treeWidget->sortByColumn(-1, Qt::AscendingOrder);
  treeWidget->setMinimumHeight(380);

  loadingIndicatorLbl = new QLabel(tr("loading ..."));
  loadingGif = new QMovie(":/studio/plugins/infrastructure/kadiintegration/img/loading.gif");
  loadingIndicatorLbl->setMovie(loadingGif);
  loadingGif->start();
  hideLoadingIndicator();

  acceptAndLoadFileBtn = new QPushButton(tr("Load Selected File"), this);
  acceptAndLoadFileBtn->setMinimumWidth(170);

  auto cancelBtn = new QPushButton(tr("Cancel"), this);
  cancelBtn->setMinimumWidth(170);

  createNewRecordBtn = new QPushButton(tr("Create New Record ..."), this);
  createNewRecordBtn->setMinimumWidth(170);
  createNewRecordBtn->setDisabled(false);
  createNewRecordBtn->setHidden(true);

  auto *formLayout = new QFormLayout(nullptr);
  formLayout->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);

  auto *hbox1 = new QHBoxLayout(nullptr);
  kadiInstanceSelectionBox = new QComboBox(this);
  hbox1->addWidget(kadiInstanceSelectionBox, 4);

  updateKadiInstanceSelectionBox();
  kadiConfigBtn = new QPushButton(this);
  kadiConfigBtn->setMaximumWidth(40);
  kadiConfigBtn->setIcon(this->style()->standardIcon(QStyle::SP_FileDialogDetailedView));
  kadiConfigBtn->setToolTip(tr("Open Kadi configuration"));
  kadiConfigBtn->setFixedHeight(kadiInstanceSelectionBox->sizeHint().height());
  QObject::connect(kadiConfigBtn, &QPushButton::clicked, this, &DownloadFromKadiDialog::showKadiConfigDialog);
  hbox1->addWidget(kadiConfigBtn, 1);

  formLayout->addRow(tr("Kadi Instance:"), hbox1);
  formLayout->addRow(tr("Results per Request:"), pageSize);
  formLayout->addRow(tr("Search Query:"), filterField);
  formLayout->addRow(tr("MimeType:"), mimeTypeFilter);

  auto *hbox2 = new QHBoxLayout(nullptr);
  hbox2->addWidget(fetchRecordsBtn);
  hbox2->addWidget(loadMoreRecordsBtn);

  formLayout->addRow(hbox2);

  auto *hbox3 = new QHBoxLayout(nullptr);
  hbox3->addWidget(createNewRecordBtn, 0, Qt::AlignLeft);
  hbox3->addWidget(loadingIndicatorLbl, 1, Qt::AlignRight);
  hbox3->addWidget(acceptAndLoadFileBtn, 0, Qt::AlignRight);
  hbox3->addWidget(cancelBtn, 0, Qt::AlignRight);

  auto *vboxLayout = new QVBoxLayout(nullptr);
  vboxLayout->addLayout(formLayout);
  vboxLayout->addWidget(treeWidget);
  vboxLayout->addLayout(hbox3);

  this->setLayout(vboxLayout);
  this->setWindowTitle(tr("Download File from Kadi"));

  acceptAndLoadFileBtn->setDisabled(true);
  connect(treeWidget, &QTreeWidget::itemClicked, acceptAndLoadFileBtn, [this](QTreeWidgetItem* clickedItem) {
    if ((operationMode == DLD_IDENTIFIER_MODE) || (operationMode == DLD_ANY_MODE) || (clickedItem->parent() != nullptr)) {
      acceptAndLoadFileBtn->setEnabled(true);
    } else {
      acceptAndLoadFileBtn->setEnabled(false);
    }
  });
  connect(fetchRecordsBtn, &QPushButton::clicked, acceptAndLoadFileBtn, [this]() { acceptAndLoadFileBtn->setDisabled(true); });
  connect(fetchRecordsBtn, &QPushButton::clicked, this, &DownloadFromKadiDialog::fetchRecordsFromKadiInstance);
  connect(createNewRecordBtn, &QPushButton::clicked, this, &DownloadFromKadiDialog::openCreateNewRecordDialog);
  connect(acceptAndLoadFileBtn, &QPushButton::clicked, this, &DownloadFromKadiDialog::accept);
  connect(cancelBtn, &QPushButton::clicked, this, &DownloadFromKadiDialog::reject);
  connect(loadMoreRecordsBtn, &QPushButton::clicked, this, [this]{ this->fetchRecordsFromKadiInstance(true); });
  connect(filterField, &QLineEdit::textEdited, loadMoreRecordsBtn, [this]{ this->loadMoreRecordsBtn->setDisabled(true); });
  connect(kadiInstanceSelectionBox, &QComboBox::activated, this, &DownloadFromKadiDialog::getSelectedKadiInstanceFromSelectionBox);

  fetchRecordsBtn->setFocus();
  filterField->setFocus();
}

bool DownloadFromKadiDialog::showDialog(int mode) {
  if (mode != operationMode) { // if there was a mode change
    getAllRecords().clear();
    treeWidget->clear();
    loadedRecords.storeRelease(0);
  }

  operationMode = mode;

  switch (operationMode) {
    case DLD_ANY_MODE:
      this->setWindowTitle(tr("Select File or Identifier from Kadi"));
      acceptAndLoadFileBtn->setText(tr("Selected File or Record"));
      createNewRecordBtn->setHidden(false);
      break;
    case DLD_FILE_MODE:
      this->setWindowTitle(tr("Download File from Kadi"));
      acceptAndLoadFileBtn->setText(tr("Load Selected File"));
      createNewRecordBtn->setHidden(true);
      break;
    case DLD_IDENTIFIER_MODE:
      setWindowTitle(tr("Select Record Identifier from Kadi"));
      acceptAndLoadFileBtn->setText(tr("Select Record"));
      createNewRecordBtn->setHidden(false);
      break;
    default:
      return false;
  }

  exec();
  mimeTypeFilter->setText("");
  return (result() == DownloadFromKadiDialog::Accepted);
}

void DownloadFromKadiDialog::applyFilter(const QString &filter) {
  if (filter.isEmpty()) {
    mimeTypeFilter->setText("");
    return;
  }
  QMimeDatabase db;
  QRegularExpression regex("(\\*\\.[^ ^)]*)");
  QRegularExpressionMatchIterator i = regex.globalMatch(filter);
  QStringList mimetypelist;
  while (i.hasNext()) {
    QRegularExpressionMatch match = i.next();
    if (match.hasMatch() && match.lastCapturedIndex() > 0) {
      QString typematch = match.captured(1);
      qDebug() << typematch;
      QMimeType type;
      QString mimetypestring;
      if (typematch == "*.flow") {
        mimetypestring = workflowmimetype;
      } else {
        type = db.mimeTypeForFile(typematch, QMimeDatabase::MatchExtension);
        if (type == db.mimeTypeForName("application/octet-stream")) {
          qDebug() << "Unknown extension " << typematch;
          continue;
        } else {
          mimetypestring = type.name();
          // qDebug() << type;
        }

      }
      // qDebug() << "This is " << mimetypestring;
      mimetypelist << mimetypestring;
    }
  }

  if (mimetypelist.size() == 0) {
    qWarning() << "No mime type for filter \"" << filter << "\" could be found.";
    mimeTypeFilter->setText("");
    return;
  } else if (mimetypelist.size() > 1) {
    qDebug() << "Only one mimetype filter is supported actually!";
  }
  mimeTypeFilter->setText(mimetypelist[0]);
}

void DownloadFromKadiDialog::showKadiConfigDialog() {
  auto kadiConfigInterface = pluginmanager->getInterface<KadiConfigInterface *>("/plugins/infrastructure/kadiconfig");
  kadiConfigInterface->showDialog(); // blocks until dialog is closed
  updateKadiInstanceSelectionBox();
}

void DownloadFromKadiDialog::updateKadiInstanceSelectionBox() {
  auto kadiConfigInterface = pluginmanager->getInterface<KadiConfigInterface *>("/plugins/infrastructure/kadiconfig");
  kadiInstanceSelectionBox->clear();
  int defaultIndex = -1;
  for (const auto& kadiInstance : kadiConfigInterface->getAllInstances()) {
    kadiInstanceSelectionBox->addItem(kadiInstance.name);
    if (kadiInstance.isDefaultInstance) {
      defaultIndex = kadiInstanceSelectionBox->count() - 1;
    }
  }
  if (defaultIndex != -1) {
    kadiInstanceSelectionBox->setCurrentIndex(defaultIndex);
    getSelectedKadiInstanceFromSelectionBox(defaultIndex);
  }
}

void DownloadFromKadiDialog::getSelectedKadiInstanceFromSelectionBox(int index) {
  auto kadiConfigInterface = pluginmanager->getInterface<KadiConfigInterface *>("/plugins/infrastructure/kadiconfig");

  currentKadiInstance = kadiConfigInterface->getAllInstances()[index];
  if (!currentKadiInstance.host.endsWith("/")) {
    currentKadiInstance.host.append("/");
  }
  currentKadiInstance.host.append(KADI_RECORDS_ENDPOINT);
}

void DownloadFromKadiDialog::fetchRecordsFromKadiInstance(bool useUrlToNextPage = false) {
  showLoadingIndicator();
  fetchRecordsBtn->setDisabled(true);
  acceptAndLoadFileBtn->setDisabled(true);
  loadMoreRecordsBtn->setDisabled(true);
  pageSize->setDisabled(true);
  filterField->setDisabled(true);
  treeWidget->sortByColumn(-1, Qt::AscendingOrder);

  // sort the list by default if no query string was given
  if (filterField->text().isEmpty()) {
    treeWidget->sortItems(0, Qt::AscendingOrder);
  }

  // url query
  QUrlQuery urlQuery;

  QNetworkRequest request;
  if (useUrlToNextPage) {
    QUrl url(urlToNextPageOfRecords);

    urlQuery = QUrlQuery(url);
    urlQuery.removeAllQueryItems("page");
    urlQuery.removeAllQueryItems("per_page");
    urlQuery.addQueryItem("page", QString::number(getNextPageNumber()));
    urlQuery.addQueryItem("per_page", pageSize->currentText());

    url.setQuery(urlQuery);
    request.setUrl(url);
  } else {
    QUrl url(currentKadiInstance.host);

    urlQuery.addQueryItem("per_page", pageSize->currentText());
    urlQuery.addQueryItem("query", filterField->text());
    if (not mimeTypeFilter->text().isEmpty()) {
      urlQuery.addQueryItem("mimetype", QUrl::toPercentEncoding(mimeTypeFilter->text()));
    }

    url.setQuery(urlQuery);
    request.setUrl(url);

    loadMoreRecordsBtn->setText(tr("Load More"));
    getAllRecords().clear();
    treeWidget->clear();
    loadedRecords.storeRelease(0);
  }
  request.setRawHeader("Authorization", (QString("Bearer ") + currentKadiInstance.token).toUtf8());

  preRequestHooks();
  //qDebug() << request.url() << endl;
  auto requesthandler = new kadiutils::NetworkHandler(networkAccessManager, this);
  connect(requesthandler, &kadiutils::NetworkHandler::finished, this, [this, requesthandler](QNetworkReply* reply) {
    processRecordsReply(reply);
    postRequestHooks();
    delete requesthandler;
  });
  requesthandler->sendRequest(request);

  filterField->setDisabled(false);
}

void DownloadFromKadiDialog::processRecordsReply(QNetworkReply* reply) {
  int statusCode = kadiutils::getHttpCode(reply);

  if (statusCode != 200) {
    fetchRecordsBtn->setText(tr("Fetch Records - HTTP Code ") + QString::number(statusCode));
    fetchRecordsBtn->setDisabled(false);
    pageSize->setDisabled(false);
    return;
  } else {
    // restore button text
    fetchRecordsBtn->setText(tr("Fetch Records"));
  }

  // parse the json in the reply body
  QJsonObject jsonobject;
  if (not kadiutils::parseJsonData(reply, jsonobject)) {
    return;
  }

  for (const QJsonValue &value : jsonobject["items"].toArray()) {
    QJsonObject item = value.toObject();
    RecordInfo recordInfo = {
      .id = item["id"].toInt(),
      .identifier = item["identifier"].toString(),
      .title = item["title"].toString(),
      .authorDisplayName = item["creator"].toObject()["displayname"].toString(),
      .description = item["description"].toString(),
      .filesApiLink = item["_links"].toObject()["files"].toString(),
      .lastModified = QDateTime::fromString(item["last_modified"].toString(), Qt::ISODate),
      .files = {},
      .filesAlreadyFetched = false,
      .type = {},
      .license = ""
    };

    // check if record was already appended to the vector
    QVector<RecordInfo> &allRecords = getAllRecords();
    if (!allRecords.contains(recordInfo)) {
      allRecords.append(recordInfo);
      int recordIndex = allRecords.size() - 1;

      // update item count
      loadedRecords++;
      updateLoadMoreRecordsBtn();

      // create record list item in tree view
      auto *newRecordListItem = new RecordListItem(treeWidget, {
        recordInfo.identifier,
        recordInfo.lastModified.toString("yyyy-MM-dd hh:mm"),
      });

      newRecordListItem->setIcon(0, this->style()->standardIcon(QStyle::SP_DirIcon));
      newRecordListItem->setToolTip(0, tr("Title: ").append(recordInfo.title).append('\n')
        .append(tr("Author: ")).append(recordInfo.authorDisplayName).append('\n')
        .append(tr("Description: ")).append(recordInfo.description.isEmpty() ? "" : "\n")
        .append(recordInfo.description));
      newRecordListItem->recordInfoIndex = recordIndex;

      // fetch files contained in this record
      if (operationMode == DLD_ANY_MODE || operationMode == DLD_FILE_MODE) {
        if (filterField->text().isEmpty()) { // lazy loading
          // add a dummy entry, so that the tree view shows an arrow to expand the item
          auto dummy = new RecordFileListItem(nullptr, { tr("loading files ..."), "" });
          newRecordListItem->addChild(dummy);

          connect(treeWidget, &QTreeWidget::itemDoubleClicked, this, &DownloadFromKadiDialog::fetchRecordFiles);
          connect(treeWidget, &QTreeWidget::itemExpanded, this, &DownloadFromKadiDialog::fetchRecordFiles);
        } else {
          fetchRecordFiles(newRecordListItem);
        }
      }
    }
  }

  totalItems = jsonobject["_pagination"].toObject()["total_items"].toInt();
  updateLoadMoreRecordsBtn();

  // pagination
  if (jsonobject["_pagination"].toObject()["_links"].toObject()["next"].isString()) {
    urlToNextPageOfRecords = jsonobject["_pagination"].toObject()["_links"].toObject()["next"].toString();
    loadMoreRecordsBtn->setDisabled(false);
  }

  fetchRecordsBtn->setDisabled(false);
  pageSize->setDisabled(false);
}

void DownloadFromKadiDialog::fetchRecordFiles(QTreeWidgetItem* recordTreeWidgetItem) {
  showLoadingIndicator();
  auto recordListItem = (RecordListItem *) recordTreeWidgetItem;
  auto &recordInfo = getAllRecords()[recordListItem->recordInfoIndex];

  if (recordInfo.filesAlreadyFetched) {
    hideLoadingIndicator();
    return;
  }

  if (recordTreeWidgetItem->childCount() == 1) {
    recordTreeWidgetItem->takeChild(0); // remove dummy
  }

  QNetworkRequest request;
  request.setUrl(QUrl(recordInfo.filesApiLink + "?per_page=100"));
  request.setRawHeader("Authorization", (QString("Bearer ") + currentKadiInstance.token).toUtf8());

  preRequestHooks();
  auto requesthandler = new kadiutils::NetworkHandler(networkAccessManager, this);
  connect(requesthandler, &kadiutils::NetworkHandler::finished, this, [this, requesthandler, recordListItem](QNetworkReply* reply) {
    processFilesReply(reply, recordListItem->recordInfoIndex, recordListItem);
    postRequestHooks();
    delete requesthandler;
  });
  requesthandler->sendRequest(request);

  recordInfo.filesAlreadyFetched = true;
  recordTreeWidgetItem->setExpanded(true);
}

void DownloadFromKadiDialog::processFilesReply(QNetworkReply *filesReply, int recordIndex, RecordListItem *parentRecordListItem) {
  int statusCode = kadiutils::getHttpCode(filesReply);

  if (statusCode > 0 && statusCode != 200) {
    return;
  }

  auto &allRecords = getAllRecords();

  // parse the json in the reply body
  QJsonObject jsonobject;
  if (not kadiutils::parseJsonData(filesReply, jsonobject)) {
    return;
  }

  for (auto value : jsonobject["items"].toArray()) {
    QJsonObject item = value.toObject();
    RecordFileInfo recordFileInfo = {
      .uuid = QUuid(item["id"].toString()),
      .fileName = item["name"].toString(),
      .lastModified = QDateTime::fromString(item["last_modified"].toString(), Qt::ISODate),
      .fileDownloadApiLink = item["_links"].toObject()["download"].toString(),
      .recordApiLink = item["_links"].toObject()["record"].toString(),
      .mimeType = item["mimetype"].toString(),
      .magicMimeType = item["magic_mimetype"].toString(),
      .checksum = item["checksum"].toString(),
      .size = item["size"].toInt(),
    };

    if (!allRecords[recordIndex].files.contains(recordFileInfo)) {
      allRecords[recordIndex].files.append(recordFileInfo);

      // create tree widget item for this file
      auto *newRecordFileListItem = new RecordFileListItem(nullptr, {
        recordFileInfo.fileName,
        //allRecords[recordIndex].title,
        recordFileInfo.lastModified.toString("yyyy-MM-dd hh:mm"),
      });

      newRecordFileListItem->setToolTip(0, QString(recordFileInfo.fileName).append('\n')
        .append(tr("Size: %1 Bytes").arg(recordFileInfo.size)).append('\n')
        .append(tr("Mime type: %1").arg(recordFileInfo.mimeType)).append('\n')
        .append(tr("Magic mime type: %1").arg(recordFileInfo.magicMimeType)).append('\n')
        .append(tr("Checksum: %1").arg(recordFileInfo.checksum).append('\n'))
        .append(tr("UUID: %1").arg(recordFileInfo.uuid.toString())));
      newRecordFileListItem->recordFileInfoIndex = allRecords[recordIndex].files.size() - 1;

      QMimeDatabase db;
      auto mimeType = db.mimeTypeForName(recordFileInfo.mimeType);
      auto magicMimeType = db.mimeTypeForName(recordFileInfo.magicMimeType);

      if (not mimeType.iconName().isEmpty() && QIcon::hasThemeIcon(mimeType.iconName())) {
        newRecordFileListItem->setIcon(0, QIcon::fromTheme(mimeType.iconName()));
      } else if (not magicMimeType.iconName().isEmpty() && QIcon::hasThemeIcon(magicMimeType.iconName())) {
        newRecordFileListItem->setIcon(0, QIcon::fromTheme(magicMimeType.iconName()));
      } else {
        newRecordFileListItem->setIcon(0, this->style()->standardIcon(QStyle::SP_FileIcon));
      }

      // add as a child to the record list item
      parentRecordListItem->addChild(newRecordFileListItem);
      if (not mimeTypeFilter->text().isEmpty()) {
        if (mimeTypeFilter->text() == workflowmimetype) {
          if (recordFileInfo.mimeType != workflowmimetype) { // magic will never match
            newRecordFileListItem->setHidden(true);
          }
        } else {
          QMimeType filtermimetype = db.mimeTypeForName(mimeTypeFilter->text());
          if (filtermimetype != mimeType && filtermimetype != magicMimeType) {
            newRecordFileListItem->setHidden(true);
          }
        }
      }
    }
  }

  // pagination
  QJsonValue paginationnext = jsonobject["_pagination"].toObject()["_links"].toObject()["next"];
  if (paginationnext.isString()) {
    auto nextPageUrl = paginationnext.toString();
    QNetworkRequest request = filesReply->request();
    request.setUrl(QUrl(nextPageUrl));

    preRequestHooks();
    auto requesthandler = new kadiutils::NetworkHandler(networkAccessManager, this);
    connect(requesthandler, &kadiutils::NetworkHandler::finished, this, [=, this](QNetworkReply* reply) {
      processFilesReply(reply, recordIndex, parentRecordListItem);
      postRequestHooks();
      delete requesthandler;
    });
    requesthandler->sendRequest(request);
  }
}

void DownloadFromKadiDialog::updateLoadMoreRecordsBtn() {
  loadMoreRecordsBtn->setText(tr("Load More (%1/%2 loaded)").arg(loadedRecords).arg(totalItems));
}

void DownloadFromKadiDialog::showLoadingIndicator() {
  loadingIndicatorLbl->setDisabled(false);
  loadingIndicatorLbl->setMovie(loadingGif);
  loadingGif->setPaused(false);
}

void DownloadFromKadiDialog::hideLoadingIndicator() {
  loadingGif->setPaused(true);
  loadingIndicatorLbl->setMovie(nullptr);
  loadingIndicatorLbl->setDisabled(true);
}

int DownloadFromKadiDialog::getNextPageNumber() {
  int per_page = pageSize->currentText().toInt();
  if (loadedRecords < per_page) {
    return 1;
  } else if (loadedRecords == per_page) {
    return 2;
  } else {
    return (int) floor(((double) loadedRecords / (double) per_page) + 1.0);
  }
}

void DownloadFromKadiDialog::preRequestHooks() {
  activeRequests.ref();
}

void DownloadFromKadiDialog::postRequestHooks() {
  activeRequests.deref();
  if (activeRequests == 0) {
    hideLoadingIndicator();
  } else {
    showLoadingIndicator();
  }
}

void DownloadFromKadiDialog::selectFile(const QString& filepath) {
  QString kadiInstanceName;
  QString recordIdentifier;
  QString kadifilename;

  if (kadiutils::getRecordIdentifier(filepath, kadiInstanceName, recordIdentifier, kadifilename)) {
    kadiInstanceSelectionBox->setCurrentText(kadiInstanceName);
    QList<QTreeWidgetItem *> recordItems = treeWidget->findItems(recordIdentifier, Qt::MatchExactly, 0);
    if (recordItems.size() > 0) {
      QTreeWidgetItem *recordItem = recordItems.first();
      recordItem->setExpanded(true);
      treeWidget->setCurrentItem(recordItem);
      if (not kadifilename.isEmpty()) {
        QList<QTreeWidgetItem *> filenameItems = treeWidget->findItems(kadifilename, Qt::MatchExactly|Qt::MatchRecursive, 0);
        if (filenameItems.size() > 0) {
          QTreeWidgetItem *filenameItem = filenameItems.first();
          treeWidget->setCurrentItem(filenameItem, 1);
          filenameItem->setSelected(true);
        }
      } else {
        recordItem->setSelected(true);
      }
    }
  }
}

void DownloadFromKadiDialog::setFilename(const QString& instancename, const QString& recordidentifier, const QString& filename) {
  QString filepath = instancename + "/" + recordidentifier + "/" + filename;
  setLoadedFileName("kadi://" + filepath);
}

void DownloadFromKadiDialog::accept() {
  acceptAndLoadFileBtn->setDisabled(true);
  auto *selectedRecord = (RecordListItem *) treeWidget->selectedItems().first();

  if (operationMode == DLD_IDENTIFIER_MODE) {
    setFilename(currentKadiInstance.name, getAllRecords()[selectedRecord->recordInfoIndex].identifier, "");
    QDialog::accept();
    return;
  }

  // the user selects a record in any mode
  if (selectedRecord->parent() == nullptr) {
    if (operationMode == DLD_ANY_MODE) {
      setFilename(currentKadiInstance.name, getAllRecords()[selectedRecord->recordInfoIndex].identifier, "");
      QDialog::accept();
    }
    return; // can't load a record
  }

  auto *selectedItem = (RecordFileListItem *) treeWidget->selectedItems().first();
  selectedRecord = (RecordListItem *) treeWidget->selectedItems().first()->parent();
  auto &recordInfo = getAllRecords()[selectedRecord->recordInfoIndex];
  RecordFileInfo &recordFileInfo = recordInfo.files[selectedItem->recordFileInfoIndex];

  if (not mimeTypeFilter->text().isEmpty() && (mimeTypeFilter->text() != recordFileInfo.mimeType && mimeTypeFilter->text() != recordFileInfo.magicMimeType)) {
    hideLoadingIndicator();
    auto result = QMessageBox::warning(this, tr("Potentially Invalid file"), tr("The selected file does not have the expected mime type. Load it anyway?"), QMessageBox::Ok | QMessageBox::Cancel, QMessageBox::Cancel);
    if (result == QMessageBox::Cancel) {
      return;
    }
  }

  setFilename(currentKadiInstance.name, recordInfo.identifier, recordFileInfo.fileName);

  QDialog::accept();
}

void DownloadFromKadiDialog::openCreateNewRecordDialog() {
  auto kadiConfigInterface = pluginmanager->getInterface<KadiConfigInterface *>("/plugins/infrastructure/kadiconfig");
  auto selectedKadiInstanceName = kadiInstanceSelectionBox->currentText();
  for (const auto& kadiInstance : kadiConfigInterface->getAllInstances()) {
    if (kadiInstance.name == selectedKadiInstanceName) {
      CreateNewRecordDialog dialog(kadiInstance);
      if (dialog.exec() == QDialog::Accepted) {
        setFilename(currentKadiInstance.name, dialog.getCreatedRecordIdentifier(), "");
        QDialog::accept();
        return;
      }
    }
  }
}

RecordListItem::RecordListItem(QTreeWidget *view, const QStringList &strings, int type) : QTreeWidgetItem(view, strings, type) {
  recordInfoIndex = -1;
}

RecordFileListItem::RecordFileListItem(QTreeWidget* view, const QStringList &strings, int type) : QTreeWidgetItem(view, strings, type) {
  recordFileInfoIndex = -1;
}
