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
#include <QSettings>
#include <QGroupBox>
#include <QFormLayout>
#include <QListWidget>
#include <QItemSelectionModel>
#include <QLineEdit>
#include <QPushButton>
#include <QCheckBox>
#include <QNetworkReply>

#include <framework/pluginframework/pluginmanagerinterface.h>

#include "kadiconfiginterface.h"
#include "kadiinstance.h"


#define KC_SECTION_GLOBAL "global"
#define KC_DEFAULT_INSTANCE "global/default"
#define KC_KEY_HOST "/host"
#define KC_KEY_TOKEN "/pat"

#define DEFAULT_TAG "[default] "

class AskSelectionModel : public QItemSelectionModel {
  Q_OBJECT
  Q_DISABLE_COPY_MOVE(AskSelectionModel)

  public:
    using QItemSelectionModel::QItemSelectionModel;

  public Q_SLOTS:
    void setCurrentIndex(const QModelIndex &index, QItemSelectionModel::SelectionFlags command) override {
      interruptselection = true;
      Q_EMIT askSelect(index, command);
    }

    void select(const QItemSelection &selection, QItemSelectionModel::SelectionFlags command) override {
      if (not interruptselection) {
        QItemSelectionModel::select(selection, command);
      }
    }
    virtual void actuallySelect(const QModelIndex &index, QItemSelectionModel::SelectionFlags command) {
      interruptselection = false;
      QItemSelectionModel::setCurrentIndex(index, command);
    }

  Q_SIGNALS:
    void askSelect(const QModelIndex &index, QItemSelectionModel::SelectionFlags command);

  private:
    bool interruptselection = false;
};


struct KadiInstanceListItem : public QListWidgetItem {
  explicit KadiInstanceListItem(const QString& text, QListWidget* view = nullptr, int type = Type);

  KadiInstance kadiInstance;
};


class KadiConfig : public QDialog, public KadiConfigInterface {
  Q_OBJECT

public:
  explicit KadiConfig(LibFramework::PluginManagerInterface* pluginmanager);

  ~KadiConfig() override = default;

  void showDialog() override;

  QList<KadiInstance>  getAllInstances() override;

  KadiInstance getDefaultInstance() override;

  void keyPressEvent(QKeyEvent* e) override;
  void closeEvent(QCloseEvent* e) override;

private:
  LibFramework::PluginManagerInterface *pluginmanager;

  QSettings *kadiConfig = nullptr;
  int nextUntitledId = 1;

  QGroupBox *listBox;
  QVBoxLayout *listBoxLayout;

  QListWidget *instanceList;

  QGroupBox *editInstanceBox;
  QFormLayout *editInstanceLayout;

  QLineEdit *nameField;
  QLineEdit *urlField;
  QLineEdit *tokenField;
  QAction *show_pat_action;

  QCheckBox *showTokenFieldBox;

  QPushButton *addBtn;
  QPushButton *removeBtn;
  QPushButton *dupeBtn;
  QPushButton *setAsDefaultBtn;
  QPushButton *testInstanceBtn;
  QPushButton *saveBtn;

  bool loadKadiConfigFromFile();

  void writeKadiConfigToFile();

  void addInstance(const QString& newName = "", const QString& host = "", const QString& token = "");

  void removeInstance();

  void duplicateInstance();

  void setInstanceAsDefault();

  bool checkForUnsavedChanges(KadiInstanceListItem* item);
  bool saveKadiConfig(KadiInstanceListItem* item);

  static QString testKadiInstance(const QString& host, const QString& token);

  void currentInstanceChanged();
};
