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
#include <QToolBar>
#include <QMenu>
#include "../domain/executionprofile.h"
#include <QPushButton>
#include <QActionGroup>
#include <QtSvgWidgets/QSvgWidget>

using ExecutionProfiles::ExecutionProfile;

/**
 * @brief      Widget to choose execution profile
 * @ingroup    widgets
 */
class ExecutionProfileChooser : public QSvgWidget {
  Q_OBJECT

  public:
    ExecutionProfileChooser();

    ExecutionProfiles::ExecutionProfile getSelectedProfile() const;
    void setSelectedProfile(ExecutionProfile selectedProfile);

    void addSelectableProfile(ExecutionProfile profile);

  Q_SIGNALS:
    void executionProfileSelected(ExecutionProfiles::ExecutionProfile profile);
    void mousePressEvent(QMouseEvent *event) override;

  private Q_SLOTS:
    void onExecutionProfileSelected(ExecutionProfiles::ExecutionProfile profile);

  private:
    QMenu *menu;
    QActionGroup *actionGroup;

    ExecutionProfile selectedProfile = ExecutionProfile::DEFAULT;
};
