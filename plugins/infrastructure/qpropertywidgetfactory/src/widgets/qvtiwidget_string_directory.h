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
#include <QLineEdit>

#include "../../qpropertywidget.h"


namespace LibFramework {
  class PluginManagerInterface;
}

class QLineEdit;
class QMenu;
class QPushButton;
class QToolButton;

class FileOpenDialogInterface;

/**
 * @brief      A widget to select a directory name by a file open dialog.
 * @ingroup    qtwidgetfactory
 */
class QPropertyFileOpen : public QPropertyWidget {
  Q_OBJECT

  public:
    QPropertyFileOpen(LibFramework::PluginManagerInterface* pluginmanager, Property* property, QWidget* parent = nullptr);
    virtual ~QPropertyFileOpen() = default;

    void synchronizeVTI() override;

  private Q_SLOTS:
    void setValue(const QString &value);
    void checkLength();

  private:
    bool validateValue() override { return true; /*TODO*/ };

    LibFramework::PluginManagerInterface *pluginmanager;

    QLineEdit *lineedit;
    QMenu *locationMenu;
    QToolButton *toolButton;
    QPushButton *validateButton;

    FileOpenDialogInterface *lastUsedDialogInterface;
    bool revalidateFilePath;
    bool validationCommandConnected;

};
