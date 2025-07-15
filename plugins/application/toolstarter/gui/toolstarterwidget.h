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

#include <memory> // unique_ptr

#include <QWidget>
#include <properties/utils/eventbus.h>
#include <framework/pluginframework/pluginmanagerinterface.h>

class QPushButton;
class QLineEdit;
class QMenu;

class Ambassador;
class PropertyFormWidgetInterface;
class ToolParameter;
class ToolChooserInterface;
class ToolProcessDialog;
class ToolHistoryDialog;
class ToolBookmarks;

/**
 * @brief      Puts all the Widgets together from the toolchooser and toolconfig classes
 *             Passes the applied signal from the toolconfig class through (start/applied button pressed)
 * @ingroup    toolstarter
 */
class ToolStarterWidget : public QWidget {
    Q_OBJECT

  public:
    explicit ToolStarterWidget(LibFramework::PluginManagerInterface *pluginmanager, QWidget *parent = nullptr);
    ~ToolStarterWidget() override;

    QMenu* getMenu();

  public Q_SLOTS:
    void startTool();
    void toolSelected();
    void toolReset();
    void toolChanged(QString toolidentificationstring);

  private:
    QMenu* createMenu();

    void transferParametersToVector(QVector<ToolParameter>& target_vector, const Ambassador* ambassador);
    QString assembleToolString();
    /**
     * Build and return an Ambassador with a property for each argument of the currently selected tool
     */
    std::unique_ptr<Ambassador> buildToolArgumentsAmbassador(const QVector<ToolParameter>& target_vector);
    void onToolArgumentsChanged();

    LibFramework::PluginManagerInterface *pluginmanager;

    ToolChooserInterface *toolchooser_interface;
    PropertyFormWidgetInterface *property_form_interface;

    QPushButton *historybutton;
    ToolHistoryDialog *toolhistorydialog;
    ToolBookmarks *tb;
    QLineEdit *toolstring_widget;
    QPushButton *start_button;

    ToolProcessDialog *dialog;

    QMenu *bookmarkmenu;

    QVector<ToolParameter> parametervector;
    std::unique_ptr<Ambassador> tool_ambassador;
    EventHandles event_handles{};
};
