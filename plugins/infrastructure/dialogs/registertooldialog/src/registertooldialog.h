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

#include <QString>
#include <QTreeWidgetItem>
#include <QDialog>

class QTreeWidget;
class QPushButton;
class QLineEdit;
class QLineEditClearable;
class QDialogButtonBox;

#include <framework/pluginframework/pluginmanagerinterface.h>

#include "registertooldialoginterface.h"


#define TOOLS_TXT_PATH ".kadistudio" + QDir::separator() + "tools.txt"


struct ExecutableInfo : public QTreeWidgetItem {
  explicit ExecutableInfo() = default;
  explicit ExecutableInfo(QTreeWidget* view, const QStringList& strings, int type = Type);
  ExecutableInfo(QString name, QString  path, bool registered, bool visible);

  void setName(QString name);
  QString getName() const;
  void setPath(QString path);
  QString getPath() const;
  void setRegistered(bool flag);
  void setVisible(bool flag);

  bool registered{};
  bool visible{};

  bool executable{};
  bool xmlhelpPresent{};
};


/**
 * @brief      Provides a dialog in which the user is able add an external tool to the tools.txt.
 * @ingroup    registertooldialog
 */
class RegisterToolDialog : public QDialog, public RegisterToolDialogInterface {
  Q_OBJECT

public:
  explicit RegisterToolDialog(LibFramework::PluginManagerInterface* pluginmanager);

  ~RegisterToolDialog() override = default;

  bool showDialog() override;

  ExecutableInfo registerTool(const QString& filepath) override;

  void checkTool(const QString &filepath, bool *executable, bool *xmlhelpPresent) override;

  void reinitialize(bool force = false);

  void accept() override;

private Q_SLOTS:
  void choosenDirectories(const QStringList& directories);

private:
  void populateAllExecutablesMap(bool force = false);
  void fillToolListTrees();
  void processToolstxtContent();
  QMap<QString, ExecutableInfo*> parseToolList(const QStringList& toolList);
  QStringList readToolstxt();

  /**
   * This function searches for a file in the directories contained in the systems PATH environmental variable.
   * @param executable The filename to search for
   * @return The first directory in which @a executable is found
   */
  QString findInSystemPath(const QString& executable);
  QMap<QString, ExecutableInfo*> findAllExecutables(const QStringList& searchDirs);
  void addExternalTool();
  void selectTools();
  void deselectTools();
  bool applyChangesAndRegisterTools();
  static bool checkToolForXmlhelp(ExecutableInfo* executableInfo);
  bool appendToolToToolstxt(const ExecutableInfo& executableInfo);
  void filterToolListTree(const QString& filter);

  LibFramework::PluginManagerInterface *pluginmanager;

  QLineEdit *searchPathEdit;
  QPushButton *addSearchPathButton;
  QLineEditClearable *searchFilterEdit;
  QPushButton *addExternalToolButton;

  QTreeWidget *availableToolsTree;
  QPushButton *selectButton;
  QPushButton *deselectButton;
  QTreeWidget *registeredToolsTree;

  QPushButton *resetListTree;
  QPushButton *clearSelectionButton;
  QDialogButtonBox *bottomButtonBox;

  QMap<QString, ExecutableInfo*> *allTools;
};
