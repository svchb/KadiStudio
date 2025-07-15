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

#include <QApplication>
#include <QMessageBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QDialogButtonBox>
#include <QLineEdit>
#include <QPushButton>
#include <QTreeWidget>
#include <QStringList>
#include <QTextStream>
#include <QSettings>
#include <QDir>
#include <QFile>
#include <QFileDialog>
#include <QtConcurrent/QtConcurrentMap>
#include <QFutureWatcher>
#include <QThreadPool>
#include <QProgressDialog>
#include <QProcess>
#include <QDomDocument>

#include <framework/enhanced/qlineeditclearable.h>

#include "searchdirectorydialog.h"

#include "registertooldialog.h"

RegisterToolDialog::RegisterToolDialog(LibFramework::PluginManagerInterface* pluginmanager) : pluginmanager(pluginmanager), allTools(nullptr) {
  QVBoxLayout *dialogLayout = new QVBoxLayout(nullptr);

  searchPathEdit = new QLineEdit(nullptr);
  searchPathEdit->setMinimumWidth(220);
  searchPathEdit->setToolTip(tr("Edit this list to search for executables in more places.\n"
                                "Separate entries with a colon."));

  addSearchPathButton = new QPushButton(tr("Change Path..."));
  addSearchPathButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
  addSearchPathButton->setToolTip(tr("This does not update the PATH environment variable globally"));

  resetListTree = new QPushButton();
  resetListTree->setIcon(this->style()->standardIcon(QStyle::SP_BrowserReload));
  resetListTree->setToolTip(tr("Reset the list, useful when tools.txt or the search paths were updated.\nWarning: Resets all non-registered external tools in the list."));
  resetListTree->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);

  searchFilterEdit = new QLineEditClearable();
  searchFilterEdit->setMinimumWidth(220);
  searchFilterEdit->setPlaceholderText(tr("Filter tools."));
  searchFilterEdit->setToolTip(tr("Filter the lists below with this field"));

  addExternalToolButton = new QPushButton(tr("Add External Tool..."));
  addExternalToolButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);

  availableToolsTree = new QTreeWidget();
  availableToolsTree->setMinimumWidth(400);
  availableToolsTree->setMinimumHeight(300);
  availableToolsTree->setRootIsDecorated(false);
  availableToolsTree->setColumnCount(2);
  availableToolsTree->setHeaderLabels({tr("Available Tools"), tr("Tool Path")});
  availableToolsTree->setColumnWidth(0, 180);
  availableToolsTree->setSelectionMode(QAbstractItemView::MultiSelection);
  availableToolsTree->sortItems(0, Qt::AscendingOrder);
  availableToolsTree->setSortingEnabled(true);

  selectButton = new QPushButton();
  selectButton->setToolTip(tr("Select this tool and move it to the right list"));
  selectButton->setIcon(this->style()->standardIcon(QStyle::SP_ArrowRight));

  deselectButton = new QPushButton();
  deselectButton->setToolTip(tr("Deselect this tool and move it to the left List"));
  deselectButton->setIcon(this->style()->standardIcon(QStyle::SP_ArrowLeft));

  registeredToolsTree = new QTreeWidget();
  registeredToolsTree->setMinimumWidth(400);
  registeredToolsTree->setMinimumHeight(300);
  registeredToolsTree->setRootIsDecorated(false);
  registeredToolsTree->setColumnCount(2);
  registeredToolsTree->setHeaderLabels({tr("Registered Tool"), tr("Tool Path")});
  registeredToolsTree->setColumnWidth(0, 180);
  registeredToolsTree->setSelectionMode(QAbstractItemView::MultiSelection);
  registeredToolsTree->sortItems(0, Qt::AscendingOrder);
  registeredToolsTree->setSortingEnabled(true);

  clearSelectionButton = new QPushButton(tr("Clear Selection"));
  clearSelectionButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

  bottomButtonBox = new QDialogButtonBox();
  bottomButtonBox->addButton(QDialogButtonBox::Cancel);
  bottomButtonBox->addButton(tr("Check And Apply"), QDialogButtonBox::AcceptRole);

  auto *hboxPathAddAndReset = new QHBoxLayout(nullptr);
  hboxPathAddAndReset->addWidget(addSearchPathButton, 0);
  hboxPathAddAndReset->addWidget(resetListTree, 0);

  auto *hboxPathRow = new QHBoxLayout(nullptr);
  hboxPathRow->addWidget(searchPathEdit, 4);
  hboxPathRow->addLayout(hboxPathAddAndReset, 1);

  auto *hboxFilterRow = new QHBoxLayout(nullptr);
  hboxFilterRow->addWidget(searchFilterEdit, 4);
  hboxFilterRow->addWidget(addExternalToolButton, 1);

  auto *formLayoutTop = new QFormLayout(nullptr);
  formLayoutTop->addRow(tr("Search Paths:"), hboxPathRow);
  formLayoutTop->addRow(tr("Filter:"), hboxFilterRow);

  auto *leftRightButtonBox = new QVBoxLayout(nullptr);
  leftRightButtonBox->addWidget(selectButton);
  leftRightButtonBox->addWidget(deselectButton);

  auto *sideBySideBox = new QHBoxLayout(nullptr);
  sideBySideBox->addWidget(availableToolsTree);
  sideBySideBox->addLayout(leftRightButtonBox);
  sideBySideBox->addWidget(registeredToolsTree);

  auto *hboxLeftBottomButtons = new QHBoxLayout(nullptr);
  hboxLeftBottomButtons->addWidget(clearSelectionButton, 0);

  auto *hboxBottomButtons = new QHBoxLayout(nullptr);
  hboxBottomButtons->addLayout(hboxLeftBottomButtons);
  hboxBottomButtons->addWidget(bottomButtonBox, 0, Qt::AlignRight);

  dialogLayout->addLayout(formLayoutTop);
  dialogLayout->addLayout(sideBySideBox);
  dialogLayout->addLayout(hboxBottomButtons);

  setLayout(dialogLayout);
  setWindowTitle(tr("Manage Registered Tools And Executables"));

  connect(addSearchPathButton, &QPushButton::clicked, this, [&] {
    SearchDirectoryDialog searchdirectorydialog(searchPathEdit->text(), this);
    QObject::connect(&searchdirectorydialog, &SearchDirectoryDialog::choosenDirectories, this, &RegisterToolDialog::choosenDirectories);
    searchdirectorydialog.exec();
  });
  connect(addExternalToolButton, &QPushButton::clicked, this, &RegisterToolDialog::addExternalTool);
  connect(searchFilterEdit, &QLineEdit::textEdited, this, &RegisterToolDialog::filterToolListTree);
  connect(searchFilterEdit, &QLineEditClearable::cleared, this, [this] {
    filterToolListTree("");
  });

  connect(resetListTree, &QPushButton::clicked, this, [this] {
    reinitialize(true);
    filterToolListTree(searchFilterEdit->text());
  });
  connect(selectButton, &QPushButton::clicked, this, &RegisterToolDialog::selectTools);
  connect(deselectButton, &QPushButton::clicked, this, &RegisterToolDialog::deselectTools);
  connect(clearSelectionButton, &QPushButton::clicked, availableToolsTree, &QTreeWidget::clearSelection);
  connect(clearSelectionButton, &QPushButton::clicked, registeredToolsTree, &QTreeWidget::clearSelection);
  connect(bottomButtonBox, &QDialogButtonBox::accepted, this, &RegisterToolDialog::accept);
  connect(bottomButtonBox, &QDialogButtonBox::rejected, this, &RegisterToolDialog::reject);

  // set focus on filter edit
  searchFilterEdit->setFocus();
}

bool RegisterToolDialog::showDialog() {
  QSettings settings(qApp->applicationName(), "/plugins/infrastructure/dialogs/registertooldialog");
  searchPathEdit->setText(settings.value("path", qgetenv("PATH")).toString());
  reinitialize(true);
  return (exec() == QDialog::Accepted);
}

ExecutableInfo RegisterToolDialog::registerTool(const QString& filepath) {
  QFileInfo fileInfo(filepath);
  ExecutableInfo result(fileInfo.fileName(), fileInfo.path(), false, false);

  if (readToolstxt().contains(result.getName())) { // already present
    result.setRegistered(true);
    return result;
  }

  if (checkToolForXmlhelp(&result)) {
    if (appendToolToToolstxt(result)) {
      result.setRegistered(true);
    }
  }

  return result;
}

void RegisterToolDialog::checkTool(const QString &filepath, bool *executable, bool *xmlhelpPresent) {
  QFileInfo fileInfo(filepath);
  ExecutableInfo executableInfo(fileInfo.fileName(), fileInfo.path(), false, false);
  checkToolForXmlhelp(&executableInfo);
  *executable = executableInfo.executable;
  *xmlhelpPresent = executableInfo.xmlhelpPresent;
}

QMap<QString, ExecutableInfo*> RegisterToolDialog::findAllExecutables(const QStringList& searchDirs) {
  QMap<QString, ExecutableInfo*> result;

  // iterate over list entries
  for (const auto& pathEntry : searchDirs) {
    QDir pathDir(pathEntry);

    // iterate over every executable file in this path entry
    for (const auto& executableName : pathDir.entryList(QDir::Files | QDir::Executable)) {
      QFileInfo executableFileInfo(pathEntry + QDir::separator() + executableName);

      // if it is a proper file or if the target of the symlink wasn't yet added, insert the entry
      if (!executableFileInfo.isSymLink() || !result.contains(executableFileInfo.canonicalFilePath())) {
        result.insert(executableFileInfo.canonicalFilePath(), new ExecutableInfo(executableName, pathEntry, false, true));
      }
    }
  }

  return result;
}

void RegisterToolDialog::populateAllExecutablesMap(bool force) {
  if (allTools == nullptr || force) {
    if (allTools != nullptr) {
      qDeleteAll(*allTools);
    }
    delete allTools; // deleting nullptr is NOP
    allTools = new QMap<QString, ExecutableInfo*>(findAllExecutables(searchPathEdit->text().split(':')));
  }
}

void RegisterToolDialog::processToolstxtContent() {
  auto toolList = readToolstxt();

  // first clear registered flag on every executable
  for (auto i = allTools->begin(); i != allTools->end(); ++i) {
    i.value()->setRegistered(false);
  }

  auto tools = parseToolList(toolList);

  for (auto j = tools.constBegin(); j != tools.constEnd(); ++j) {
    allTools->insert(j.key(), j.value());
  }
}

void RegisterToolDialog::fillToolListTrees() {
  availableToolsTree->clear();
  registeredToolsTree->clear();
  for (auto i = allTools->constBegin(); i != allTools->constEnd(); ++i) {
    if (i.value()->registered) {
      registeredToolsTree->addTopLevelItem((QTreeWidgetItem *) i.value()); // overloaded operator, therefore odd notation
    } else {
      availableToolsTree->addTopLevelItem((QTreeWidgetItem *) i.value()); // overloaded operator, therefore odd notation
    }
  }
}

void RegisterToolDialog::reinitialize(bool force) {
  availableToolsTree->clearSelection();
  registeredToolsTree->clearSelection();
  populateAllExecutablesMap(force);
  processToolstxtContent();
  fillToolListTrees();
}

void RegisterToolDialog::accept() {
  if (applyChangesAndRegisterTools()) {
    QSettings settings(qApp->applicationName(), "/plugins/infrastructure/dialogs/registertooldialog");
    settings.setValue("path", searchPathEdit->text());
    QDialog::accept();
  }
}

void RegisterToolDialog::choosenDirectories(const QStringList& directories) {
  searchPathEdit->setText(directories.join(SearchDirectoryDialog::pathseparator));
  reinitialize(true);
}

QMap<QString, ExecutableInfo*> RegisterToolDialog::parseToolList(const QStringList& toolList) {
  QMap<QString, ExecutableInfo*> result;

  for (const auto& toolname : toolList) {
    if (toolname.trimmed().startsWith("#")) {
      continue;
    }

    if (toolname.contains(QDir::separator())) {  // a tool with absolute path, not located in PATH
      QFileInfo qFileInfo(toolname);
      result.insert(qFileInfo.canonicalFilePath(), new ExecutableInfo(qFileInfo.fileName(), qFileInfo.absolutePath(), true, true));
      continue;
    }

    QString containingPath = findInSystemPath(toolname);
    if (containingPath.isEmpty()) {
      result.insert(toolname, new ExecutableInfo(toolname, "", true, true));
    } else {
      result.insert(containingPath + QDir::separator() + toolname, new ExecutableInfo(toolname, containingPath, true, true));
    }
  }

  return result;
}

QStringList RegisterToolDialog::readToolstxt() {
  QStringList result;

  QFile toolstxtFile(QDir::homePath() + QDir::separator() + TOOLS_TXT_PATH);
  if (not toolstxtFile.exists()) {
    QMessageBox::warning(this, tr("Could not open tools.txt"), tr("The file '%1' does not exist.").arg(toolstxtFile.fileName()));
    return result;
  }

  if (toolstxtFile.open(QFile::ReadOnly | QFile::Text)) {
    QTextStream in(&toolstxtFile);

    while (!in.atEnd()) {
      auto line = in.readLine();
      if (!line.isEmpty()) {
        result.append(line);
      }
    }

    toolstxtFile.close();
  } else {
    QMessageBox::warning(this, tr("Could not open tools.txt"), tr("The file '%1' could not be opened for reading.").arg(toolstxtFile.fileName()));
  }

  return result;
}

QString RegisterToolDialog::findInSystemPath(const QString& executable) {
  for (const auto& systemPathEntry : QString(qgetenv("PATH")).split(SearchDirectoryDialog::pathseparator)) {
    if (QFile::exists(systemPathEntry + QDir::separator() + executable)) {
      return systemPathEntry;
    }
  }
  return QString(); // not found, return empty string
}

void RegisterToolDialog::addExternalTool() {
  if (allTools == nullptr) {
    populateAllExecutablesMap();
  }

  QStringList files = QFileDialog::getOpenFileNames(this, tr("Add one or more tools"), QDir::homePath());

  for (const auto& file : files) {
    std::cout << file.toStdString() << std::endl;
    QFileInfo qFileInfo(file);
    auto* executableInfo = new ExecutableInfo(qFileInfo.fileName(), qFileInfo.absolutePath(), true, true);

    allTools->insert(qFileInfo.filePath(), executableInfo);
    registeredToolsTree->addTopLevelItem(executableInfo);
    registeredToolsTree->scrollToItem(executableInfo);
    executableInfo->setSelected(true);
  }
}

void RegisterToolDialog::selectTools() {
  auto selection = availableToolsTree->selectedItems();
  if (selection.isEmpty()) {
    return;
  }

  // disable sorting for better performance
  availableToolsTree->setSortingEnabled(false);
  registeredToolsTree->setSortingEnabled(false);

  for (auto item : selection) {
    if (item->isHidden()) {
      continue;
    }

    // swap lists
    registeredToolsTree->addTopLevelItem(availableToolsTree->takeTopLevelItem(availableToolsTree->indexOfTopLevelItem(item)));
    ((ExecutableInfo *) item)->setRegistered(true);
  }

  // reenable sorting
  availableToolsTree->setSortingEnabled(true);
  registeredToolsTree->setSortingEnabled(true);

  // select items again in this loop. This is excluded from the loop above because of performance issues
  for (auto item : selection) {
    if (item->isHidden()) {
      continue;
    }
    //registeredToolsTree->setItemSelected(item, true);
    item->setSelected(true);
  }

  registeredToolsTree->scrollToItem(selection.first());
}

void RegisterToolDialog::deselectTools() {
  auto selection = registeredToolsTree->selectedItems();
  if (selection.isEmpty()) {
    return;
  }

  for (auto item : selection) {
    if (item->isHidden()) {
      continue;
    }

    // swap lists
    availableToolsTree->addTopLevelItem(registeredToolsTree->takeTopLevelItem(registeredToolsTree->indexOfTopLevelItem(item)));
    ((ExecutableInfo *) item)->setRegistered(false);
  }
}

bool RegisterToolDialog::applyChangesAndRegisterTools() {
  // 1. check what changed: a) what got removed b) what got added
  // 2. inform user about removals and additions
  // 3. check additions for executable status and for xmlhelp
  // 4. report results to user
  // 5. ask user to confirm changes
  // 6. write tools.txt file

  auto currentToolstxtContent = parseToolList(readToolstxt());
  QMap<QString, ExecutableInfo*> changesMade;
  QList<ExecutableInfo*> additions;
  QList<ExecutableInfo*> removals;

  for (auto i = allTools->constBegin(); i != allTools->constEnd(); ++i) {
    if (i.value()->registered) {
      changesMade.insert(i.key(), i.value());
      if (!currentToolstxtContent.contains(i.key())) {
        additions.append(i.value());
      }
    }
  }

  for (auto i = currentToolstxtContent.constBegin(); i != currentToolstxtContent.constEnd(); ++i) {
    if (!changesMade.contains(i.key())) {
      removals.append(i.value());
    }
  }

  if (removals.isEmpty() && additions.empty()) {
    // nothing changed, nothing to apply
    return true;
  }

  QString userInformationtext;
  QTextStream textStream(&userInformationtext);
  if (!removals.isEmpty()) {
    textStream << tr("The following tools should be removed") + ":" << Qt::endl;
    for (const auto& item : removals) {
      textStream << item->getName() << Qt::endl;
    }
    textStream << Qt::endl;
  }

  if (!additions.isEmpty()) {
    textStream << tr("The following tools were added and will be checked for\nthe --xmlhelp option") + ":" << Qt::endl;
    for (const auto& item : additions) {
      textStream << item->getName() << Qt::endl;
    }
    textStream << Qt::endl;
  }

  QMessageBox changesMsgBox(QMessageBox::Icon::Question,
                            tr("Changes To Apply"), tr("%1  tool(s) will be checked for the --xmlhelp parameter and %2 tool(s) will be removed. Continue?")
                            .arg(QString::number(additions.count()), QString::number(removals.count())),
                            QMessageBox::Ok | QMessageBox::Cancel, this);
  changesMsgBox.setDetailedText(userInformationtext);

  if (changesMsgBox.exec() == QMessageBox::Cancel) {
    return false;
  }

  // check for xmlhelp (and executable status)

  // Create a progress dialog.
  QProgressDialog dialog;
  dialog.setLabelText(tr("Checking tools using %1 thread(s)...").arg(QThread::idealThreadCount()));

  // Create a QFutureWatcher and connect signals and slots.
  // Monitor progress changes of the future
  QFutureWatcher<void> futureWatcher;
  QObject::connect(&futureWatcher, SIGNAL(finished()), &dialog, SLOT(reset()));
  QObject::connect(&dialog, SIGNAL(canceled()), &futureWatcher, SLOT(cancel()));
  QObject::connect(&futureWatcher, SIGNAL(progressRangeChanged(int,int)), &dialog, SLOT(setRange(int,int)));
  QObject::connect(&futureWatcher, SIGNAL(progressValueChanged(int)), &dialog, SLOT(setValue(int)));

  // Start the computation.
  futureWatcher.setFuture(QtConcurrent::map(additions, RegisterToolDialog::checkToolForXmlhelp));

  // Display the dialog and start the event loop.
  dialog.exec();

  futureWatcher.waitForFinished();

  if (futureWatcher.future().isCanceled()) {
    return false;
  }

  bool additionsValid = true;
  for (const auto& item : additions) {
    if (!item->xmlhelpPresent) {
      additionsValid = false;
      break;
    }
  }

  if (!additionsValid) {
    int countOfInvalidTools = 0;
    userInformationtext.clear();
    userInformationtext.append(tr("The following new tools do not appear to support the '--xmlhelp' command line argument") + ":\n");
    for (const auto& item : additions) {
      if (!item->xmlhelpPresent) {
        countOfInvalidTools++;
        textStream << item->getName() << ":\t"+ tr("executable") + ": " << (item->executable ? tr("yes") : tr("no")) << " xmlhelp: " + tr("no") << Qt::endl;
      }
    }

    QMessageBox continueWithValidMsgBox(QMessageBox::Icon::Question, tr("Incompatible Tool(s) Found"),
      tr("%1 incompatible tool(s) found. Continue without these tools? Press OK to register all valid tools or press Cancel to not register any tool at all.")
      .arg(QString::number(countOfInvalidTools)),
      QMessageBox::Ok | QMessageBox::Cancel, this);
    continueWithValidMsgBox.setDetailedText(userInformationtext);
    if (continueWithValidMsgBox.exec() == QMessageBox::Cancel) {
      return false;
    }

    // remove invalid tools from changes
    for (const auto& item : additions) {
      if (!item->xmlhelpPresent) {
        changesMade.remove(item->getPath() + QDir::separator() + item->getName());
      }
    }
  }

  // finally write all changes to tools.txt
  QFile toolstxtFile(QDir::homePath() + QDir::separator() + TOOLS_TXT_PATH);
  if (not toolstxtFile.open(QFile::WriteOnly | QFile::Truncate | QFile::Text)) {
    QMessageBox::warning(this, tr("Could not write tools.txt"), tr("File '%1' could not be opened for writing.").arg(toolstxtFile.fileName()));
    return false;
  }

  QTextStream out(&toolstxtFile);

  QStringList searchpathlist = QString(qgetenv("PATH")).split(':');
  for (const auto &itemToRegister : changesMade.values()) {
    if (searchpathlist.contains(itemToRegister->getPath())
        || itemToRegister->getPath().isEmpty()) {
      out << itemToRegister->getName() << Qt::endl;
    } else {
      out << itemToRegister->getPath() << QDir::separator() << itemToRegister->getName() << Qt::endl;
    }
  }

  toolstxtFile.close();

  return true;
}

bool RegisterToolDialog::checkToolForXmlhelp(ExecutableInfo* executableInfo) {
  QString toolFilePath(executableInfo->getPath() + QDir::separator() + executableInfo->getName());
  QProcess proc;

  // check executable status
  QFileInfo toolFileInfo(toolFilePath);
  executableInfo->executable = toolFileInfo.isExecutable();

  if (!executableInfo->executable) {
    return false; // if not executable don't check for xmlhelp
  }

  // check for xmlhelp
  proc.start(toolFilePath, { "--xmlhelp" });
  if (!proc.waitForFinished()) { // timeout, check unsuccessful
    qWarning() << "Timeout when trying to request XML description";
    proc.kill();
    return false;
  }

  QString output = proc.readAllStandardOutput();
  proc.close();

  QDomDocument testdoc;
  executableInfo->xmlhelpPresent = testdoc.setContent(output);

  return executableInfo->xmlhelpPresent;
}

bool RegisterToolDialog::appendToolToToolstxt(const ExecutableInfo& executableInfo) {
  QFile toolstxtFile(QDir::homePath() + QDir::separator() + TOOLS_TXT_PATH);

  if (not toolstxtFile.open(QFile::Append | QFile::Text)) {
    QMessageBox::warning(this, tr("Could not write tools.txt"), tr("File '%1' could not be opened for appending.").arg(toolstxtFile.fileName()));
    return false;
  }
  QTextStream out(&toolstxtFile);
  if (QString(qgetenv("PATH")).split(':').contains(executableInfo.getPath())) {
    out << executableInfo.getName();
  } else {
    out << executableInfo.getPath() << QDir::separator() << executableInfo.getName();
  }
  out << Qt::endl;
  toolstxtFile.close();
  return true;
}

void RegisterToolDialog::filterToolListTree(const QString& filter) {
  for (auto i = allTools->constBegin(); i != allTools->constEnd(); ++i) {
    if (i.key().contains(filter, Qt::CaseInsensitive)) {
      i.value()->setVisible(true);
    } else {
      i.value()->setVisible(false);
    }
  }
}

ExecutableInfo::ExecutableInfo(QTreeWidget* view, const QStringList& strings, int type)
  : QTreeWidgetItem(view, strings, type), registered(false), visible(false), executable(false), xmlhelpPresent(false) {
}

ExecutableInfo::ExecutableInfo(QString name, QString path, bool registered, bool visible)
  : QTreeWidgetItem((QTreeWidget *) nullptr, { "" }), registered(registered), visible(visible), executable(false), xmlhelpPresent(false) {
  setName(name);
  setPath(path);
  setRegistered(this->registered);
  setVisible(this->visible);
}

void ExecutableInfo::setName(QString name) {
  setText(0, name);
}

QString ExecutableInfo::getName() const {
  return text(0);
}

void ExecutableInfo::setPath(QString path) {
  setText(1, path);
}

QString  ExecutableInfo::getPath() const {
  return text(1);
}

void ExecutableInfo::setRegistered(bool flag) {
  registered = flag;
}

void ExecutableInfo::setVisible(bool flag) {
  visible = flag;
  setHidden(!visible);
}
