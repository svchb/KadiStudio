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

#include <QMenu>

/**
 * @brief      A specialized QMenu which automatically sets a shortcut to its default action.
 *             The default action (and the shortcut) will automatically be set to the action which was used last.
 * @ingroup    src
 */
class ShortcutMenu : public QMenu {

  public:
    explicit ShortcutMenu(const QString& name, const QKeySequence& shortcut);

    /**
     * Sets the default action using `QMenu::setDefault(QAction*)`, and sets the shortcut for this action.
     * If there was a default action before, then this function removes the shortcut from it.
     * @param action New default action
     */
    void setDefaultActionWithShortcut(QAction *action);

  protected:
    void actionEvent(QActionEvent* event) override;

  private:
    QKeySequence shortcut;
};
