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

#include <framework/enhanced/qlineeditclearable.h>
#include <QTreeWidgetItem>
#include <QDialog>

#include <plugins/infrastructure/kadiconfig/kadiinstance.h>
#include "../../downloadfromkadidialoginterface.h"

#include "../domain/recordinfo.h"

// forward declarations
namespace LibFramework {
  class PluginManagerInterface;
}

class QAtomicInt;
class QComboBox;
class QLabel;
class QMovie;
class QNetworkAccessManager;
class QNetworkReply;
class QString;


struct RecordListItem : public QTreeWidgetItem {
  explicit RecordListItem(QTreeWidget* view, const QStringList& strings, int type = Type);

  int recordInfoIndex;
};


struct RecordFileListItem : public QTreeWidgetItem {
  explicit RecordFileListItem(QTreeWidget* view, const QStringList& strings, int type = Type);

  int recordFileInfoIndex;
};


/**
 * @brief      Show the dialog in which the user is able to download a file from Kadi.
 * @ingroup    kadiintegration
 */
class DownloadFromKadiDialog : public QDialog, public DownloadFromKadiDialogInterface {
Q_OBJECT

public:
  explicit DownloadFromKadiDialog(LibFramework::PluginManagerInterface* pluginmanager, QWidget* parent = nullptr);

  virtual ~DownloadFromKadiDialog() = default;

  QDialog* getDialog() override {
    return this;
  }

  bool showDialog(int mode) override;

  void applyFilter(const QString& filter) override;

  QVector<RecordInfo>& getAllRecords() override {
    return recordsCache;
  }

  void selectFile(const QString& filepath) override;
  void setFilename(const QString& instancename, const QString& recordidentifier, const QString& filename) override;

  const QString& getLoadedFileName() const override {
    return loadedFileName;
  }

private:
  LibFramework::PluginManagerInterface *pluginmanager;

  KadiInstance currentKadiInstance;

  int operationMode = 0;

  QNetworkAccessManager *networkAccessManager;
  QAtomicInt activeRequests;

  QComboBox *kadiInstanceSelectionBox;
  QPushButton *kadiConfigBtn;
  QComboBox *pageSize;
  QLineEdit *filterField;
  QLineEditClearable *mimeTypeFilter;
  QPushButton *fetchRecordsBtn;
  QPushButton *loadMoreRecordsBtn;
  QTreeWidget *treeWidget;
  QMovie *loadingGif;
  QLabel *loadingIndicatorLbl;
  QPushButton *createNewRecordBtn;
  QPushButton *acceptAndLoadFileBtn;

  QString urlToNextPageOfRecords;
  QAtomicInt loadedRecords;
  QAtomicInt totalItems;

  QString loadedFileName;

  void setLoadedFileName(const QString &fileName) {
    loadedFileName = fileName;
  }

  void showKadiConfigDialog();

  void updateKadiInstanceSelectionBox();

  void getSelectedKadiInstanceFromSelectionBox(int index);

  void fetchRecordsFromKadiInstance(bool useUrlToNextPage);

  void processRecordsReply(QNetworkReply *reply);

  void fetchRecordFiles(QTreeWidgetItem *recordTreeWidgetItem);

  void processFilesReply(QNetworkReply *filesReply, int recordIndex,
                         RecordListItem *parentRecordListItem);

  void updateLoadMoreRecordsBtn();

  void showLoadingIndicator();

  void hideLoadingIndicator();

  int getNextPageNumber();

  void preRequestHooks();

  void postRequestHooks();

  void accept() Q_DECL_OVERRIDE;

  void openCreateNewRecordDialog();

  QVector<RecordInfo> recordsCache{};

};
