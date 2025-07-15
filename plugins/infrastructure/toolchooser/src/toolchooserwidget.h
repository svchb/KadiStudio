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

#include <QWidget>
#include <QMultiMap>
#include <QString>

#include "tooldata/toolxmldata.h"
#include "../toolchooserinterface.h"

class QPushButton;
class QTreeWidget;
class QTreeWidgetItem;

#define CONFIG_FOLDER ".kadistudio"
#define CONFIG_TOOLS_FILENAME "tools.txt"

class QLineEditClearable;
class InfoTabWidget;

/**
 * @brief      Allows to select a tool in a specific version.
 * @ingroup    tooldialog
 */
class ToolChooserWidget : public QWidget, public ToolChooserInterface {
    Q_OBJECT

  public:
    explicit ToolChooserWidget();
    ~ToolChooserWidget() override;

    ToolChooserInterface* clone() const override {
      return new ToolChooserWidget();
    }

    QWidget* getWidget() override;

    void setTool(const QString& toolidentificationstring) override;

    const ToolDescription& getToolDescription(const QString& toolidentificationstring) override {
      toolxml.createXML(toolidentificationstring);
      return toolxml.Description();
    }

    const ToolDescription& getToolDescription() const override {
      return toolxml.Description();
    }
    /**
     * Build and return a map of all relevant information about the tool (name, path, arguments...)
     */
    QMap<QString, QVariant> getToolData() override;

    void setToolSelectedCallback(std::function<void()> function) override;
    void setToolResetCallback(std::function<void()> function) override;

  Q_SIGNALS:
    void toolSelected();
    void toolReset();

  private Q_SLOTS:
    void initTreeItems();
    void filterTools(QString text);
    void resetFilterTools();
    void toolHovered(QTreeWidgetItem *item, int column);
    void selectTool(QTreeWidgetItem *toolnameitem, int column);
    void toolVersionSelected(const QString &version);
    void resizeColumns(QTreeWidgetItem *item);
    void configureTools();

  private:
    void updateDirContent(QTreeWidgetItem *toolnameitem, const QString &path);

    ToolXMLData toolxml;
    bool externaltool;
    QRegularExpression rxversionedtool;
    QList<QTreeWidgetItem *> toplevelletteritems;
    QMultiMap<QTreeWidgetItem *, QString> comboboxentries;
    QTreeWidgetItem *lastselectedtoolitem;

    QLineEditClearable *searchbarwidget;
    QTreeWidget *dircontent;
    InfoTabWidget *infotabwidget;

    std::function<void()> toolselected_callback;
    std::function<void()> toolreset_callback;

};
