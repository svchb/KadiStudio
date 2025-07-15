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

#include <QDebug>
#include <QLayout>
#include <QMessageBox>
#include <QDir>
#include <QTreeWidget>
#include <QComboBox>
#include <QPushButton>
#include <QSplitter>
#include <QTextStream>

#include <framework/enhanced/qlineeditclearable.h>

#include <framework/pluginframework/pluginmanagerinterface.h>
#include <framework/pluginframework/pluginmanager.h>

#include <plugins/infrastructure/dialogs/registertooldialog/registertooldialoginterface.h>

#include <properties/data/ambassador.h>

#include "infotabwidget.h"
#include "tooldata/tooldescription.h"
#include "toolchooserwidget.h"

ToolChooserWidget::ToolChooserWidget()
    : QWidget(),
      toolxml() {

  lastselectedtoolitem = NULL;
  rxversionedtool = QRegularExpression("\\-[0-9]{8}$");

  QVBoxLayout *widgetlayout = new QVBoxLayout;
  this->setLayout(widgetlayout);
  this->setMinimumWidth(300);

  QHBoxLayout *searchbarlayout = new QHBoxLayout;

  searchbarwidget = new QLineEditClearable(this);
  searchbarwidget->setPlaceholderText(tr("Filter tools."));
  connect(searchbarwidget, SIGNAL(textEdited(QString)), this, SLOT(filterTools(QString)));
  connect(searchbarwidget, SIGNAL(cleared()), this, SLOT(resetFilterTools()));
  searchbarlayout->addWidget(searchbarwidget);

  QPushButton *configbutton = new QPushButton();
  configbutton->setIcon(QIcon(":/studio/framework/pixmaps/configure.svg"));
  configbutton->setToolTip(tr("Manage registered tools"));
  configbutton->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
  connect(configbutton, SIGNAL(clicked()), this, SLOT(configureTools()));
  searchbarlayout->addWidget(configbutton);

  QPushButton *reloadbutton = new QPushButton();
  reloadbutton->setIcon(this->style()->standardIcon(QStyle::SP_BrowserReload));
  reloadbutton->setToolTip(tr("Reload " CONFIG_TOOLS_FILENAME));
  reloadbutton->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
  connect(reloadbutton, SIGNAL(clicked()), this, SLOT(initTreeItems()));
  searchbarlayout->addWidget(reloadbutton);

  widgetlayout->addLayout(searchbarlayout);

  dircontent = new QTreeWidget;
  dircontent->setColumnCount(2);
  dircontent->setSortingEnabled(true);
  dircontent->sortByColumn(0, Qt::AscendingOrder);
  dircontent->setHeaderLabels(QStringList() << "Tools" << "Release date");
  dircontent->setMouseTracking(true);
  dircontent->setAnimated(true);

  infotabwidget = new InfoTabWidget();

  connect(dircontent, &QTreeWidget::itemEntered, this, &ToolChooserWidget::toolHovered);
  connect(dircontent, &QTreeWidget::itemExpanded, this, &ToolChooserWidget::resizeColumns);
  connect(dircontent, &QTreeWidget::itemCollapsed, this, &ToolChooserWidget::resizeColumns);

  QSplitter *splitter = new QSplitter(Qt::Vertical);
  splitter->addWidget(dircontent);
  splitter->addWidget(infotabwidget);
  QList<int> sizes;
  sizes.append(dircontent->height() + infotabwidget->height() - infotabwidget->minimumHeight());
  sizes.append(infotabwidget->minimumHeight());
  splitter->setSizes(sizes);
  widgetlayout->addWidget(splitter);

  initTreeItems();

  connect(dircontent, &QTreeWidget::itemSelectionChanged, this, [this] () {
    auto selectedItems = dircontent->selectedItems();
    if (!selectedItems.isEmpty()) {
      auto selection = selectedItems[0]; // only one item can be selected at a time
      selectTool(selection, 0); // column is unused by selectTool
    } else {
      if (lastselectedtoolitem) {
        dircontent->removeItemWidget(lastselectedtoolitem, 1);
        lastselectedtoolitem = nullptr;
        infotabwidget->clear();
        Q_EMIT toolReset();
      }
    }
  });

  connect(this, &ToolChooserWidget::toolSelected, this, [this]() {
    if (toolselected_callback) {
      toolselected_callback();
    }
  });
  connect(this, &ToolChooserWidget::toolReset, this, [this] () {
    if (toolreset_callback) {
      toolreset_callback();
    }
  });
}

ToolChooserWidget::~ToolChooserWidget() {
}

QWidget *ToolChooserWidget::getWidget() {
  return this;
}

void ToolChooserWidget::updateDirContent(QTreeWidgetItem *toolItem, const QString &path) {
  QString toolname = toolItem->text(0);
  QString firstletteruppercase = toolname.at(0).toUpper();
  QTreeWidgetItem *toplevelletteritem;
  bool toplevelitemnotaddedyet = (dircontent->findItems(firstletteruppercase, Qt::MatchExactly).isEmpty());

  if (toplevelitemnotaddedyet) {
    toplevelletteritem = new QTreeWidgetItem(QStringList() << firstletteruppercase);
    // there is always an invisible root item
    bool firstItem = ((dircontent->topLevelItemCount() == 1) || (dircontent->topLevelItemCount() == 0));

    if (firstItem) {
      // -> set height of first item to height of combo box
      int comboboxheight = 25;
      toplevelletteritem->setSizeHint(0, QSize(5, comboboxheight));
      // all rows in the tree sync their heights with the first item
      // -> all items have same height as combox box
      // -> stops newly selected items from adjusting size due to combo box
      dircontent->setUniformRowHeights(true);
    }

    dircontent->addTopLevelItem(toplevelletteritem);
    toplevelletteritems.append(toplevelletteritem);
    bool searchfieldinuse = (not searchbarwidget->text().isEmpty());

    if (searchfieldinuse) {
      // hide new items by default to not disturb the search results with additional nodes
      toplevelletteritem->setHidden(true);
    }
  } else {
    toplevelletteritem = dircontent->findItems(firstletteruppercase, Qt::MatchExactly).first();
  }

  bool toolnotaddedyet = true;
  for (int j = 0; j < toplevelletteritem->childCount(); ++j) {
    QTreeWidgetItem *child = toplevelletteritem->child(j);

    if (child->text(0) == toolname) {
      toolnotaddedyet = false;
      break;
    }
  }

  if (toolnotaddedyet) {
    bool versionedtool = toolname.contains(rxversionedtool);

    if (versionedtool) {
      QString nonversionedtool = toolname.section(rxversionedtool, 0, 0);
      int index = -1;

      for (int j = 0; j < toplevelletteritem->childCount(); ++j) {
        QTreeWidgetItem *child = toplevelletteritem->child(j);

        if (child->text(0) == nonversionedtool) {
          index = j;
          break;
        }
      }

      QString releasedate = toolname.split("-").last();
      QString comboboxentryreleasedate = releasedate + " - " + path;

      if (index != -1) {
        QTreeWidgetItem *toolnameitem = toplevelletteritem->child(index);
        comboboxentries.insert(toolnameitem, comboboxentryreleasedate);
      } else {
        toolItem->setText(0, nonversionedtool);
        toplevelletteritem->addChild(toolItem);
        comboboxentries.insert(toolItem, comboboxentryreleasedate);
      }
    } else {
      toplevelletteritem->addChild(toolItem);
      QString comboboxentrycurrent = "current - " + path;
      comboboxentries.insert(toolItem, comboboxentrycurrent);
    }
  }
}

void ToolChooserWidget::initTreeItems() {
  infotabwidget->clear();
  toplevelletteritems.clear();
  comboboxentries.clear();
  dircontent->clear();
  lastselectedtoolitem = nullptr;
  toolxml.resetCache();

  QFileInfoList tools;
  QDir config_folder = QDir(QDir::home().absolutePath() + QDir::separator() + CONFIG_FOLDER + QDir::separator());
  QFile toolsFile(config_folder.absolutePath() + QDir::separator() + CONFIG_TOOLS_FILENAME);
  if (toolsFile.open(QIODevice::ReadOnly)) {
    QTextStream in(&toolsFile);
    while (!in.atEnd()) {
      QString line = in.readLine().trimmed();
      if (line.isEmpty() || line.startsWith("#")) {
        continue;
      }
      QString path, toolName;
      if (line.startsWith(QDir::separator())) {
        int splitPosition = line.lastIndexOf(QDir::separator());
        path = line.left(splitPosition);
        toolName = line.right(line.size() - splitPosition - 1);
      } else {
        toolName = line;
      }
      auto *toolitem = new QTreeWidgetItem(QStringList() << toolName);
      updateDirContent(toolitem, line);
    }
    toolsFile.close();
  } else {
    QFileInfo fileInfo(toolsFile.fileName());
    qWarning() << "Can not read tool list from: " << fileInfo.absoluteFilePath();
  }
}

void ToolChooserWidget::resizeColumns(QTreeWidgetItem *item) {
  Q_UNUSED(item);
  dircontent->resizeColumnToContents(0);
}

void ToolChooserWidget::resetFilterTools() {
  filterTools("");
}

void ToolChooserWidget::filterTools(QString text) {
  Q_FOREACH (QTreeWidgetItem * toplevelletteritem, toplevelletteritems) {
    toplevelletteritem->setHidden(false);
    int childrenHidden = 0;

    for (int j = 0; j < toplevelletteritem->childCount(); ++j) {
      QTreeWidgetItem *toolitem = toplevelletteritem->child(j);

      if (text.isEmpty() && toolitem->isHidden()) {
        toolitem->setHidden(false);
      } else if (toolitem->text(0).contains(text)) {
        toolitem->setHidden(false);
        toplevelletteritem->setExpanded(true);
      } else {
        toolitem->setHidden(true);
        childrenHidden++;
      }
    }

    bool allchildrenhidden = (childrenHidden == toplevelletteritem->childCount());
    if (allchildrenhidden) {
      toplevelletteritem->setHidden(true);
    }

    if (text.isEmpty()) {
      bool otheritemselected = false;

      for (int j = 0; j < toplevelletteritem->childCount(); ++j) {
        QTreeWidgetItem *toolitem = toplevelletteritem->child(j);
        otheritemselected = toolitem->isSelected();
        if (otheritemselected) break;
      }

      toplevelletteritem->setExpanded(otheritemselected);
    }
  }
}

void ToolChooserWidget::toolHovered(QTreeWidgetItem *toolnameitem, int column) {
  return; // disabled for now (TODO needs to run in background)
  Q_UNUSED(column);
  bool istoolitem = (toolnameitem->childCount() == 0);

  const ToolDescription *tooldescription = nullptr;

  if (istoolitem) {
    QString name = toolnameitem->text(0);
    QString path;

    if (toolnameitem->isSelected()) {
      // if tool is selected split the selected version path
      QComboBox *box = dynamic_cast<QComboBox *>(dircontent->itemWidget(toolnameitem, 1));
      path = box->currentText().split(" - ").at(1);
    } else {
      // if not selected assume the path of first version
      QStringList boxentries = comboboxentries.values(toolnameitem);
      path = boxentries.at(0).split(" - ").at(1);
    }

    if (toolxml.createToolDescription(path)) {
      tooldescription = &toolxml.Description();
    }

    dircontent->setSelectionMode(QAbstractItemView::SingleSelection);

  } else {
    dircontent->setSelectionMode(QAbstractItemView::NoSelection);

    if (lastselectedtoolitem != nullptr) {
      tooldescription = &toolxml.Description();
    }
  }

  infotabwidget->setToolInformation(tooldescription);
}

void ToolChooserWidget::selectTool(QTreeWidgetItem *toolnameitem, int column) {
  Q_UNUSED(column);
  bool istoolitem = (toolnameitem->childCount() == 0);

  if (istoolitem) {
    if (lastselectedtoolitem != nullptr) {
      lastselectedtoolitem->setSelected(false);
      dircontent->removeItemWidget(lastselectedtoolitem, 1);
    }

    QComboBox *combobox = new QComboBox;
    QStringList boxentries = comboboxentries.values(toolnameitem);

    for (int i = boxentries.size() - 1; i >= 0; i--) {
      combobox->addItem(boxentries.at(i));
    }

    combobox->setAutoFillBackground(true);
    connect(combobox, SIGNAL(currentTextChanged(QString)), this, SLOT(toolVersionSelected(QString)));
    dircontent->setItemWidget(toolnameitem, 1, combobox);

    lastselectedtoolitem = toolnameitem;

    QString version = combobox->currentText();
    toolVersionSelected(version); // emits signals
  }
}

void ToolChooserWidget::toolVersionSelected(const QString &versionWithPath) {
  QTreeWidgetItem *toolitem = lastselectedtoolitem;

  // adjust size of combo box to show tool path completely
  QComboBox *combobox = dynamic_cast<QComboBox *>(dircontent->itemWidget(toolitem, 1));
  combobox->adjustSize();
  dircontent->setColumnWidth(1, combobox->width());

  QString toolname = toolitem->text(0);
  QString path = versionWithPath.split(" - ").last();
  QString version = versionWithPath.split(" - ").first();

  if (version != "current") {
    toolname = toolname + "-" + version;
  }

  bool valid = toolxml.createToolDescription(path);

  // always show description tab by selecting a tool
  infotabwidget->setCurrentIndex(1);
  infotabwidget->setToolInformation(&toolxml.Description());

  if (valid) {
    Q_EMIT toolSelected();
  } else {
    Q_EMIT toolReset();
    QMessageBox::critical(this, tr("Error"), tr("Could not retrieve xmlhelp for this tool"));
  }
}

void ToolChooserWidget::setTool(const QString &toolidentificationstring) {

  if (toolxml.createXML(toolidentificationstring)) {
    QString name = toolxml.Description().name();
    QString shortname = toolxml.Description().shortName();
    QString toolname = shortname.split(" - ").first();

    bool toolfound = false;

    Q_FOREACH (QTreeWidgetItem * toplevelletteritem, toplevelletteritems) {
      toplevelletteritem->setHidden(false);

      for (int j = 0; !toolfound && j < toplevelletteritem->childCount(); ++j) {
        QTreeWidgetItem *toolnameitem = toplevelletteritem->child(j);

        if (toolnameitem->text(0).contains(toolname)) {
          if (lastselectedtoolitem != nullptr) {
            lastselectedtoolitem->setSelected(false);
            dircontent->removeItemWidget(lastselectedtoolitem, 1);
          }

          // maybe it is filtered out so make it visible again
          toolnameitem->setHidden(false);
          toplevelletteritem->setHidden(false);

          toplevelletteritem->setExpanded(true);

          bool oldState = dircontent->blockSignals(true);
          toolnameitem->setSelected(true);
          dircontent->blockSignals(oldState);
          dircontent->scrollToItem(toolnameitem);

          lastselectedtoolitem = toolnameitem;

          QComboBox *combobox = new QComboBox;
          combobox->setAutoFillBackground(true);
          dircontent->setItemWidget(toolnameitem, 1, combobox);

          QStringList boxentries = comboboxentries.values(toolnameitem);
          for (int i = boxentries.size() - 1; i >= 0; i--) {
            combobox->addItem(boxentries.at(i));
          }

          for (int index = 0; index < combobox->count(); index++) {
            QString comboboxtext = combobox->itemText(index);
            QStringList comboboxsplit = comboboxtext.split(" - ");
            QString comboboxversionstring = comboboxsplit.at(0);
            QString comboboxpathstring = comboboxsplit.at(1);

            if (name == comboboxpathstring) {
              combobox->setCurrentIndex(index);
              toolfound = true;
              break;
            }
          }
          connect(combobox, SIGNAL(currentTextChanged(QString)), this, SLOT(toolVersionSelected(QString)));
          combobox->adjustSize();
          dircontent->setColumnWidth(1, combobox->width());

          infotabwidget->setToolInformation(&toolxml.Description());
          // always show description tab by selecting a tool
          infotabwidget->setCurrentIndex(1);
        }
      }
    }

    if (toolfound) {
      Q_EMIT toolSelected();
    } else {
      Q_EMIT toolReset();
      QMessageBox::warning(this, tr("Error"), tr("Tool '%1' is not registered").arg(toolidentificationstring));
    }
  } else {
    Q_EMIT toolReset();
  }
}

void ToolChooserWidget::configureTools() {
  LibFramework::PluginManagerInterface *pluginmanager = LibFramework::PluginManager::getInstance();
  auto registerToolDialogInterface = pluginmanager->getInterface<RegisterToolDialogInterface *>("/plugins/infrastructure/dialogs/registertooldialog");
  if (registerToolDialogInterface->showDialog()) {
    initTreeItems();
  }
}

QMap<QString, QVariant> ToolChooserWidget::getToolData() {
  QMap<QString, QVariant> tooldatamap;
  const ToolDescription &tooldescription = toolxml.Description();
  QString program = tooldescription.name();
  QString toolstring = program;
  QList<QVariant> allarguments;

  for (auto parameter : tooldescription.parameterVector()) {
    allarguments << parameter.toJson();
  }

  tooldatamap.insert("toolstring", QVariant(toolstring));
  tooldatamap.insert("toolexecutable", QVariant(program));
  tooldatamap.insert("toolarguments", QVariant(allarguments));
  tooldatamap.insert("toolname", QVariant(tooldescription.shortName()));
  tooldatamap.insert("toolpath", QVariant(tooldescription.getToolpath()));

  tooldatamap.insert("description", QVariant(tooldescription.description()));
  tooldatamap.insert("example", QVariant(tooldescription.example()));
  tooldatamap.insert("version", QVariant(tooldescription.version()));
  tooldatamap.insert("env", QVariant(tooldescription.isEnv()));

  // tooldatamap.insert("xmlhelp", QVariant(tooldescription.getXmlhelp()));

  return tooldatamap;
}

void ToolChooserWidget::setToolSelectedCallback(std::function<void()> function) {
  toolselected_callback = function;
};

void ToolChooserWidget::setToolResetCallback(std::function<void()> function) {
  toolreset_callback = function;
};
