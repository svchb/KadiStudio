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

#include "properties/data/propertiesmodel.h"
#include <QWidget>
#include <QLabel>
#include <plugins/infrastructure/qpropertywidgetfactory/widgetfactoryinterface.h>

class Ambassador;


class DebugLabelListener : public PropertyChangeListener {
  public:
    DebugLabelListener(QLabel* label) : label(label) {};
    void receivePropertyChange(const Property* property) override {
      label->setText(QString::fromStdString(property->getName()) + " : " + QString::fromStdString(property->toString()));
    }

    void receivePropertyChange(const Property* property, const std::string &path) override {
      label->setText(QString::fromStdString(property->getName()) + " : " + QString::fromStdString(property->toString() + ", path=" + path));
    }

  private:
    QLabel *label;
};

class PropertiesTestGui : public QWidget {
    Q_OBJECT

  public:
    PropertiesTestGui(WidgetFactoryInterface* widgetFactoryInterface, QWidget* parent = NULL);
    virtual ~PropertiesTestGui() = default;

    Ambassador* getAmbassador() {
      return model;
    }

  private:
    PropertiesModel *model;

};
