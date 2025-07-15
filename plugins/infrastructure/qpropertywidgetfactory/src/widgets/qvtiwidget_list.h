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

#include <QListWidget>

#include "../../qvtiwidget.h"

/**
 * @brief      A list widget.
 * @ingroup    qtwidgetfactory
 */
class QVTIListWidget : public QVTIWidget {
  Q_OBJECT

  public:

    QVTIListWidget(AbstractValueTypeInterface* avti, QWidget* parent = NULL);
    virtual ~QVTIListWidget() = default;

    void synchronizeVTI() override;

  private:
    QListWidget* getQListWidget() {
      return reinterpret_cast<QListWidget*>(getWidget());
    }

};
