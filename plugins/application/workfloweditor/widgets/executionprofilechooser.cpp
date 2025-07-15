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

#include "executionprofilechooser.h"
#include <QStylePainter>
#include <QStyleOptionButton>
#include <QMouseEvent>

ExecutionProfileChooser::ExecutionProfileChooser() : menu(new QMenu("Execution profile")),
                                                     actionGroup(new QActionGroup(this)) {
  setAttribute(Qt::WA_OpaquePaintEvent);
  setFixedWidth(18);
  setFixedHeight(18);
  onExecutionProfileSelected(selectedProfile);
  menu->addSection("Execution Profile");

  connect(this, &ExecutionProfileChooser::mousePressEvent, menu, [&](QMouseEvent* /*event*/) {
    menu->popup(QCursor::pos());
  });
  connect(this, &ExecutionProfileChooser::executionProfileSelected, this,
          &ExecutionProfileChooser::onExecutionProfileSelected);
}

ExecutionProfile ExecutionProfileChooser::getSelectedProfile() const {
  return selectedProfile;
}

void ExecutionProfileChooser::setSelectedProfile(ExecutionProfile profile) {
  selectedProfile = profile;
  emit executionProfileSelected(profile);
}

void ExecutionProfileChooser::addSelectableProfile(ExecutionProfile profile) {
  auto *profileAction = new QAction(ExecutionProfiles::name(profile));
  connect(profileAction, &QAction::triggered, this, [&, profile]() {
    selectedProfile = profile;
  });
  connect(profileAction, &QAction::triggered, this, [&](bool) {
    emit executionProfileSelected(selectedProfile);
  });
  profileAction->setActionGroup(actionGroup);
  profileAction->setIcon(QIcon(ExecutionProfiles::iconPath(profile)));
  profileAction->setToolTip(ExecutionProfiles::help(profile));
  menu->addAction(profileAction);
}

void ExecutionProfileChooser::onExecutionProfileSelected(ExecutionProfiles::ExecutionProfile profile) {
  load(ExecutionProfiles::iconPath(profile));
  setToolTip(ExecutionProfiles::help(profile));
}
