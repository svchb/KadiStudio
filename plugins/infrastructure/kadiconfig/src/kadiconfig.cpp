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

#include <QtWidgets>
#include <unistd.h>

#include <QCloseEvent>
#include <QAction>
#include <QFile>
#include <QTextStream>
#include <QIcon>
#include <QMessageBox>
#include <QProcess>
#include <QDir>
#include <QTimer>

#include <framework/enhanced/qlineeditclearable.h>

#include <QNetworkRequest>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QEventLoop>

#include "kadiconfig.h"

KadiConfig::KadiConfig(LibFramework::PluginManagerInterface* pluginmanager) : pluginmanager(pluginmanager) {
  setWindowTitle(tr("Kadi Configuration"));
  auto dialogLayout = new QVBoxLayout();

  listBox = new QGroupBox(tr("Kadi Instances"));
  listBoxLayout = new QVBoxLayout();

  instanceList = new QListWidget();
  instanceList->setBaseSize(150, 100);
  instanceList->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding));

  AskSelectionModel *askModel = new AskSelectionModel(instanceList->model(), instanceList);
  instanceList->setSelectionModel(askModel);

  connect(askModel, &AskSelectionModel::askSelect, [this, askModel](const QModelIndex &index, QItemSelectionModel::SelectionFlags command) {
    if (index == instanceList->currentIndex()) return;
    auto *previousitem = dynamic_cast<KadiInstanceListItem *>(instanceList->currentItem());
    if (previousitem == nullptr || checkForUnsavedChanges(previousitem)) {
      if (previousitem) {
        // might have changed but should be ignored
        QString name;
        if (previousitem->kadiInstance.isDefaultInstance) name += DEFAULT_TAG;
        name += previousitem->kadiInstance.name;
        previousitem->setText(name);
      }
      askModel->actuallySelect(index, command);
    }
  });

  addBtn = new QPushButton();
  addBtn->setIcon(QIcon(":/studio/plugins/infrastructure/kadiconfig/icons/fa-plus-solid.svg"));
  addBtn->setToolTip(tr("Add an instance"));
  removeBtn = new QPushButton();
  // removeBtn->setIcon(QIcon(":/studio/plugins/infrastructure/kadiconfig/icons/fa-minus-solid.svg"));
  removeBtn->setIcon(QIcon(":/studio/plugins/infrastructure/kadiconfig/icons/fa-trash-can-solid.svg"));
  removeBtn->setToolTip(tr("Remove this instance"));
  dupeBtn = new QPushButton();
  dupeBtn->setToolTip(tr("Duplicate this instance"));
  dupeBtn->setIcon(QIcon(":/studio/plugins/infrastructure/kadiconfig/icons/fa-clone-regular.svg"));
  dupeBtn->setFixedHeight(removeBtn->sizeHint().height());
  setAsDefaultBtn = new QPushButton(tr("Set as Default"));
  setAsDefaultBtn->setToolTip(tr("Set this instance as the default one"));

  listBoxLayout->addWidget(instanceList);

  auto *hbox1 = new QHBoxLayout();
  hbox1->addWidget(addBtn);
  hbox1->addWidget(dupeBtn);
  hbox1->addWidget(removeBtn);
  hbox1->addWidget(setAsDefaultBtn);

  listBoxLayout->addLayout(hbox1);

  editInstanceBox = new QGroupBox(tr("Kadi Instance Editor"));
  editInstanceLayout = new QFormLayout();

  int minimumwidth = 600;
  nameField = new QLineEditClearable();
  nameField->setMinimumWidth(minimumwidth);
  nameField->setToolTip(tr("Name of Kadi Instance"));
  connect(nameField, &QLineEditClearable::textChanged, this, [this]() {
    auto *item = dynamic_cast<KadiInstanceListItem *>(instanceList->currentItem());
    saveBtn->setDisabled(item == nullptr || item->kadiInstance.name == nameField->text());
    if (item) {
      QString name;
      if (item->kadiInstance.isDefaultInstance) name += DEFAULT_TAG;
      name += nameField->text();
      item->setText(name);
    }
  });

  urlField = new QLineEditClearable();
  urlField->setMinimumWidth(minimumwidth);
  urlField->setToolTip(tr("URL to Kadi Instance"));
  urlField->setPlaceholderText("https://...");
  connect(urlField, &QLineEditClearable::textChanged, this, [this]() {
    auto *item = dynamic_cast<KadiInstanceListItem *>(instanceList->currentItem());
    saveBtn->setDisabled(item == nullptr || item->kadiInstance.host == urlField->text());
  });

  tokenField = new QLineEditClearable();
  tokenField->setMinimumWidth(minimumwidth);
  tokenField->setToolTip(tr("Personal Access Token"));
  tokenField->setEchoMode(QLineEdit::Password);
  connect(tokenField, &QLineEditClearable::textChanged, this, [this]() {
    auto *item = dynamic_cast<KadiInstanceListItem *>(instanceList->currentItem());
    saveBtn->setDisabled(item == nullptr || item->kadiInstance.token == tokenField->text());
  });

  show_pat_action = tokenField->addAction(QIcon(":/studio/plugins/infrastructure/kadiconfig/icons/fa-eye-slash-regular.svg"), QLineEdit::TrailingPosition);

  connect(show_pat_action, &QAction::triggered, this, [=, this]() {
    if (tokenField->echoMode() == QLineEdit::Password) {
      tokenField->setEchoMode(QLineEdit::Normal);
      show_pat_action->setIcon(QIcon(":/studio/plugins/infrastructure/kadiconfig/icons/fa-eye-regular.svg"));
    } else {
      tokenField->setEchoMode(QLineEdit::Password);
      show_pat_action->setIcon(QIcon(":/studio/plugins/infrastructure/kadiconfig/icons/fa-eye-slash-regular.svg"));
    }
  });

  testInstanceBtn = new QPushButton(tr("Test Instance"));
  testInstanceBtn->setIcon(this->style()->standardIcon(QStyle::SP_ArrowRight));

  saveBtn = new QPushButton(tr("Save Changes"));
  saveBtn->setIcon(this->style()->standardIcon(QStyle::SP_DialogSaveButton));
  saveBtn->setDisabled(true);

  auto *savetestbuttonbox = new QHBoxLayout();
  savetestbuttonbox->addWidget(testInstanceBtn);
  savetestbuttonbox->addWidget(saveBtn);

  editInstanceLayout->addRow("Name:", nameField);
  editInstanceLayout->addRow("Host:", urlField);
  editInstanceLayout->addRow("PAT:", tokenField);
  editInstanceLayout->addRow(savetestbuttonbox);

  listBox->setLayout(listBoxLayout);
  editInstanceBox->setLayout(editInstanceLayout);

  auto configLayout = new QHBoxLayout();
  configLayout->addWidget(listBox);
  configLayout->addWidget(editInstanceBox);

  dialogLayout->addLayout(configLayout);

  auto bottomLayout = new QHBoxLayout();
  QSpacerItem *item2 = new QSpacerItem(1,1, QSizePolicy::Expanding, QSizePolicy::Fixed);
  bottomLayout->addItem(item2);
  auto *closebutton = new QPushButton(tr("Close"));
  closebutton->setIcon(this->style()->standardIcon(QStyle::SP_DialogCloseButton));
  connect(closebutton, &QPushButton::pressed, this, [=, this]() {
    this->close();
  });

  bottomLayout->addWidget(closebutton);
  dialogLayout->addLayout(bottomLayout);

  setLayout(dialogLayout);

  connect(addBtn, &QPushButton::clicked, this, [=, this]{ if (checkForUnsavedChanges(dynamic_cast<KadiInstanceListItem *>(instanceList->currentItem()))) {this->addInstance();} });
  connect(removeBtn, &QPushButton::clicked, this, &KadiConfig::removeInstance);
  connect(dupeBtn, &QPushButton::clicked, this, &KadiConfig::duplicateInstance);
  connect(setAsDefaultBtn, &QPushButton::clicked, this, &KadiConfig::setInstanceAsDefault);
  connect(saveBtn, &QPushButton::clicked, this, [this]() {
    saveKadiConfig(dynamic_cast<KadiInstanceListItem *>(instanceList->currentItem()));
  });
  connect(instanceList, &QListWidget::currentItemChanged, this, &KadiConfig::currentInstanceChanged);
  connect(testInstanceBtn, &QPushButton::clicked, this, [=, this] {
    if (urlField->text().isEmpty() || tokenField->text().isEmpty()) {
      QMessageBox::information(this, tr("Test Kadi Instance"), tr("You need to specify a host and a token."));
    } else {
      auto errorMsg = testKadiInstance(urlField->text(), tokenField->text());
      if (errorMsg.isEmpty()) {
        testInstanceBtn->setText(tr("VALID!"));
        QTimer::singleShot(4000, testInstanceBtn, [=, this] { testInstanceBtn->setText(tr("Test Instance")); });
      } else {
        QMessageBox::warning(this, tr("Test Kadi Instance"), tr("The test was not successful, the following error occurred:\n\n") + errorMsg);
      }
    }
  });

  loadKadiConfigFromFile();  // load config here so that when only the interface is used the data can be fetched
}

void KadiConfig::showDialog() {
  if (loadKadiConfigFromFile()) {
    exec();
  } else {
    // config file is needed to continue, so don't show the dialog if no config is there
  }
}

QList<KadiInstance>  KadiConfig::getAllInstances() {
  QList<KadiInstance> result;

  for (int i = 0; i < instanceList->count(); ++i) {
    auto item = (KadiInstanceListItem *) instanceList->item(i);
    KadiInstance instance {
      .name = item->kadiInstance.name,
      .host = item->kadiInstance.host,
      .token = item->kadiInstance.token,
      .isDefaultInstance = item->kadiInstance.isDefaultInstance,
    };
    result.push_back(instance);
  }

  return result;
}

KadiInstance KadiConfig::getDefaultInstance() {
  auto defaultValue = kadiConfig->value("global/default", "").toString();
  return KadiInstance {
    .name = defaultValue,
    .host = kadiConfig->value(defaultValue + KC_KEY_HOST).toString(),
    .token = kadiConfig->value(defaultValue + KC_KEY_TOKEN).toString(),
    .isDefaultInstance = true,
  };
}

bool KadiConfig::loadKadiConfigFromFile() {
  auto kadiConfigFile = QDir::homePath().append("/.kadiconfig");
  if (access(kadiConfigFile.toStdString().c_str(), F_OK | R_OK | W_OK) == -1) {
    if (errno == ENOENT) {  // No such file or directory
      QMessageBox createConfigMsgBox(this);
      createConfigMsgBox.setIcon(QMessageBox::Warning);
      createConfigMsgBox.setWindowTitle(tr("No Configuration Found"));
      createConfigMsgBox.setText(
        tr("The file '%1' doesn't exist, should it be created using kadi-apy?\n\n"
        "Notice: The file is needed to continue. If you choose to not create the configuration file you "
        "will need to create it manually or retry by opening the Kadi config dialog again.").arg(kadiConfigFile)
        );
      createConfigMsgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel | QMessageBox::Escape);
      createConfigMsgBox.setDefaultButton(QMessageBox::Ok);

      if (createConfigMsgBox.exec() == QMessageBox::Ok) {
        QProcess proc;
        proc.start("kadi-apy", {
          "config",
          "create"
        });
        proc.waitForFinished(10000);
        if (proc.exitCode() != 0) {
          QMessageBox::warning(this, tr("Configuration Generation Failed"), tr("'kady-apy config create' failed:\n\n") + proc.readAllStandardOutput());
          return false;
        }
        // show the dialog so that the user can edit the invalid default instance created by `kadi-apy config create` immediately
        // this won't create an infinite recursion since the .kadiconfig file will be present after this
        showDialog();
      } else {
        return false;  // do not create a new .kadiconfig file
      }
    } else {
      QMessageBox::warning(this, tr("Reading Configuration Failed"), tr("The file '%1' is not accessible.").arg(kadiConfigFile));
      qWarning() << kadiConfigFile << " is not accessible, access() failed with error no. " << strerror(errno);
      return false;
    }
  }

  delete kadiConfig;
  kadiConfig = new QSettings(kadiConfigFile, QSettings::IniFormat);

  instanceList->clear();

  auto defaultValue = kadiConfig->value("global/default", "");
  KadiInstanceListItem *initiallySelectedItem = nullptr;

  // get all instances
  for (const auto &s : kadiConfig->childGroups()) {
    if (s == "global") {
      continue;
    }

    auto newListItem = new KadiInstanceListItem(s, instanceList);
    newListItem->kadiInstance.name = s;
    newListItem->kadiInstance.host = kadiConfig->value(s + KC_KEY_HOST).toString();
    newListItem->kadiInstance.token = kadiConfig->value(s + KC_KEY_TOKEN).toString();

    if (s == defaultValue) {
      initiallySelectedItem = newListItem;
      newListItem->kadiInstance.isDefaultInstance = true;
      newListItem->setText(DEFAULT_TAG + s);
    }

    if (!initiallySelectedItem) {
      initiallySelectedItem = newListItem;
    }
  }

  // set the values of the editor for the first item
  if (initiallySelectedItem) {
    instanceList->setCurrentItem(initiallySelectedItem);
    initiallySelectedItem->setSelected(true);
  }

  return true; // success
}

static void unescapeIniKey(QString& iniKey) {
  int index = 0;
  QString result;
  result.reserve(iniKey.size());

  while (index < iniKey.length()) {
    auto c = iniKey[index];

    if (c == '%' && (iniKey.length() - index) >= 3) {
      bool numberConversionSuccessful = false;
      ushort ch = iniKey.mid(index + 1, 2).toUShort(&numberConversionSuccessful, 16);

      if (!numberConversionSuccessful) {
        result.append("%");
        ++index;
        continue;
      }

      QChar qChar(ch);
      result.append(qChar);
      index += 3;
    } else {
      result.append(c);
      ++index;
    }
  }

  iniKey = result;
}

void KadiConfig::writeKadiConfigToFile() {
  kadiConfig->sync();

  QString fileContent;

  // read the file again and unescape the section key names (e.g.: "[Demo%20Instanz]" -> "[Demo Instanz]")
  QFile file(kadiConfig->fileName());
  if (file.open(QIODevice::ReadOnly)) {
    QTextStream in(&file);

    while (!in.atEnd()) {
      QString line = in.readLine();

      if (line.startsWith('[')) {
        unescapeIniKey(line);
      }
      fileContent.append(line).append('\n');
    }

    file.close();
  }

  // finally overwrite it again with corrected section names
  if (file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
    QTextStream out(&file);
    out << fileContent;
    file.close();
  }
}

void KadiConfig::addInstance(const QString& newName, const QString& host, const QString& token) {
  QString name;
  if (newName.isEmpty()) {
    do {
      name = tr("unnamed") + " " + QString::number(nextUntitledId++);
    } while (!instanceList->findItems(name, Qt::MatchExactly).isEmpty());
  } else {
    name = newName;
  }

  kadiConfig->setValue(name + KC_KEY_HOST, host);
  kadiConfig->setValue(name + KC_KEY_TOKEN, token);
  writeKadiConfigToFile();

  auto newListItem = new KadiInstanceListItem(name, instanceList);
  newListItem->kadiInstance.name = name;
  newListItem->kadiInstance.host = kadiConfig->value(name + KC_KEY_HOST).toString();
  newListItem->kadiInstance.token = kadiConfig->value(name + KC_KEY_TOKEN).toString();

  instanceList->setCurrentItem(newListItem);
}

void KadiConfig::removeInstance() {
  auto *item = dynamic_cast<KadiInstanceListItem *>(instanceList->currentItem());
  if (!item) {
    return;
  }

  QMessageBox::StandardButton reply;
  reply = QMessageBox::question(this, tr("Kadi Configuration"), tr("Are you sure you want to delete this instance?"),
                                QMessageBox::Ok|QMessageBox::Cancel);
  if (reply == QMessageBox::Cancel) {
    return;
  }

  kadiConfig->remove(item->kadiInstance.name + KC_KEY_HOST);
  kadiConfig->remove(item->kadiInstance.name + KC_KEY_TOKEN);

  if (item->kadiInstance.isDefaultInstance) {
    delete item;
    auto *firstItemInList = (KadiInstanceListItem *) instanceList->item(0);
    if (firstItemInList) {
      firstItemInList->kadiInstance.isDefaultInstance = true;
      firstItemInList->setText(DEFAULT_TAG + firstItemInList->kadiInstance.name);
      kadiConfig->setValue(KC_DEFAULT_INSTANCE, firstItemInList->kadiInstance.name);
    } else {
      // no more items in the list
      kadiConfig->setValue(KC_DEFAULT_INSTANCE, "");
    }
  } else {
    delete item;
  }

  writeKadiConfigToFile();
}

void KadiConfig::duplicateInstance() {
  auto *item = dynamic_cast<KadiInstanceListItem *>(instanceList->currentItem());
  if (!item) {
    return;
  }
  if (not checkForUnsavedChanges(dynamic_cast<KadiInstanceListItem *>(instanceList->currentItem()))) return;

  QString newName(item->kadiInstance.name);
  do {
    newName = newName + " " + tr("(copy)");
  } while (!instanceList->findItems(newName, Qt::MatchExactly).isEmpty());

  this->addInstance(newName, item->kadiInstance.host, item->kadiInstance.token);
}

void KadiConfig::setInstanceAsDefault() {
  auto *item = dynamic_cast<KadiInstanceListItem *>(instanceList->currentItem());
  if (!item || item->kadiInstance.isDefaultInstance) {
    return;
  }

  auto foundItems = instanceList->findItems(DEFAULT_TAG, Qt::MatchStartsWith);
  if (!foundItems.isEmpty()) {
    auto *oldDefaultItem = (KadiInstanceListItem *) foundItems.first();
    oldDefaultItem->kadiInstance.isDefaultInstance = false;
    oldDefaultItem->setText(oldDefaultItem->kadiInstance.name);
  }

  item->kadiInstance.isDefaultInstance = true;
  item->setText(DEFAULT_TAG + item->kadiInstance.name);

  kadiConfig->setValue(KC_DEFAULT_INSTANCE, item->kadiInstance.name);
  writeKadiConfigToFile();
}

bool KadiConfig::saveKadiConfig(KadiInstanceListItem* item) {
  if (kadiConfig == nullptr) {
    // TODO: prompt to create new config with kadi-apy
    return false;
  }

  if (!item) {
    // TODO: add new item here maybe?
    return false;
  }

  for (int i = 0; i < instanceList->count(); ++i) {
    auto testitem = (KadiInstanceListItem *) instanceList->item(i);
    if (testitem != item && testitem->kadiInstance.name == nameField->text()) {
      QMessageBox::warning(this, tr("Kadi Configuration"),
                                 tr("Name '%1' for instance already in use.\nPlease choose another name.").arg(nameField->text()),
                           QMessageBox::Ok);
      nameField->setFocus();
      return false;
    }
  }

  if (item->kadiInstance.name != nameField->text()) {
    // section renamed
    kadiConfig->remove(item->kadiInstance.name + KC_KEY_HOST);
    kadiConfig->remove(item->kadiInstance.name + KC_KEY_TOKEN);
    item->kadiInstance.name = nameField->text();

    // rename list item and default instance in config file
    if (item->kadiInstance.isDefaultInstance) {
      item->setText(DEFAULT_TAG + item->kadiInstance.name);
      kadiConfig->setValue(KC_DEFAULT_INSTANCE, item->kadiInstance.name);
    } else {
      item->setText(item->kadiInstance.name);
    }
  }

  // update item
  item->kadiInstance.host = urlField->text();
  item->kadiInstance.token = tokenField->text();

  // update kadi config
  kadiConfig->setValue(item->kadiInstance.name + KC_KEY_HOST, urlField->text());
  kadiConfig->setValue(item->kadiInstance.name + KC_KEY_TOKEN, tokenField->text());

  writeKadiConfigToFile();
  saveBtn->setText(tr("Changes successfully saved."));
  saveBtn->setDisabled(true);
  QTimer::singleShot(4000, saveBtn, [this] { saveBtn->setText(tr("Save Changes")); });

  return true;
}

QString KadiConfig::testKadiInstance(const QString& host, const QString& token) {
  QNetworkAccessManager networkAccessManager;

  auto url = host + (host.endsWith("/") ? "" : "/") + "api/records";
  QNetworkRequest request((QUrl(url)));
  request.setRawHeader("Authorization", (QString("Bearer ") + token).toUtf8());

  auto *reply = networkAccessManager.get(request);
  QEventLoop eventLoop;
  connect(reply, &QNetworkReply::finished, &eventLoop, &QEventLoop::quit);
  eventLoop.exec();

  if (reply->error() == QNetworkReply::NoError) {
    return {""};
  } else {
    return reply->errorString();
  }
}

bool KadiConfig::checkForUnsavedChanges(KadiInstanceListItem* item) {
  if (item != nullptr &&
      (item->kadiInstance.name  != nameField->text() ||
       item->kadiInstance.host  != urlField->text()  ||
       item->kadiInstance.token != tokenField->text())  ) {
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, tr("Kadi Configuration"), tr("You have changed entries.\nDo you want to save them?"),
                                  QMessageBox::Save|QMessageBox::Ignore|QMessageBox::Cancel);
    switch (reply) {
      case QMessageBox::Save:
        if (saveKadiConfig(item)) break;
        [[fallthrough]];

      case QMessageBox::Cancel: {
        return false;
      }

      default:
        ; // ignore just goes through
    }
  }
  return true;
}

void KadiConfig::currentInstanceChanged() {
  // reset widgets to default
  testInstanceBtn->setText(tr("Test Instance"));
  saveBtn->setText(tr("Save Changes"));
  tokenField->setEchoMode(QLineEdit::Password);
  show_pat_action->setIcon(QIcon(":/studio/plugins/infrastructure/kadiconfig/icons/fa-eye-slash-regular.svg"));

  auto *item = dynamic_cast<KadiInstanceListItem *>(instanceList->currentItem());
  if (!item) {
    nameField->clear();
    urlField->clear();
    tokenField->clear();
    editInstanceBox->setDisabled(true);
  } else {
    nameField->setText(item->kadiInstance.name);
    urlField->setText(item->kadiInstance.host);
    tokenField->setText(item->kadiInstance.token);
    saveBtn->setDisabled(true);
    editInstanceBox->setDisabled(false);
  }
}

void KadiConfig::keyPressEvent(QKeyEvent* e) {
  if (e->key() == Qt::Key_Escape && not checkForUnsavedChanges(dynamic_cast<KadiInstanceListItem *>(instanceList->currentItem()))) {
    return;
 }

  QDialog::keyPressEvent(e);
}

void KadiConfig::closeEvent(QCloseEvent* e) {
  if (not checkForUnsavedChanges(dynamic_cast<KadiInstanceListItem *>(instanceList->currentItem()))) {
    e->ignore();
  } else {
    QDialog::closeEvent(e);
  }
}

KadiInstanceListItem::KadiInstanceListItem(const QString& text, QListWidget* view, int type) : QListWidgetItem(text, view, type) {
  kadiInstance = KadiInstance {
    .name = "",
    .host = "",
    .token = "",
    .isDefaultInstance = false,
  };
}
