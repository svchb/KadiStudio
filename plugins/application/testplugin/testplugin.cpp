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

#include <framework/pluginframework/pluginmanager.h>
#include <framework/statusbar/statusbarinterface.h>
#include <framework/menu/menudelegate.h>
#include <framework/dock/dockdelegate.h>
#include <framework/dock/dockwindow.h>
#include <framework/tab/tabdelegate.h>
#include <plugins/infrastructure/qpropertywidgetfactory/widgetfactoryinterface.h>
#include <QDebug>

#include "gui/propertiestestgui.h"
#include "testplugin.h"

#include <plugins/infrastructure/settings/settingsinterface.h>

#define P_NAMESPACE "/plugins/application/testplugin"

void TestPlugin::run() {
  if (filename) {
    qDebug("Given filename: %s", filename);
  }

  qDebug("test: %li", test);

  DockInterface *dockinterface = DockDelegate::getInstance();
  dockinterface->addDockWindow(P_NAMESPACE, dockwindow);

  TabInterface *tabinterface = TabDelegate::getInstance();
  tabinterface->addTab(P_NAMESPACE, sTestGui, "TestPlugin");

  MenuInterface *menuinterface = MenuDelegate::getInstance();
  menuinterface->addMenu(P_NAMESPACE, menu);

  // Get Settings Interface
  SettingsInterface *settingsinterface = getPluginManager()->getInterface<SettingsInterface *>("/plugins/infrastructure/settingsplugin");

  // Add some settings
  settingsinterface->setPluginSetting(P_NAMESPACE, "string_setting", "This is a String");
  settingsinterface->setPluginSetting(P_NAMESPACE, "bool_setting", true);
  settingsinterface->setPluginSetting(P_NAMESPACE, "int_setting", 42);

  // Retrieve a setting
  int foo = settingsinterface->getPluginSetting(P_NAMESPACE, "int_setting", 42).toInt();

  // Create Configuration Menu
  settingsinterface->addConfigMenu(P_NAMESPACE, menu);

}

void TestPlugin::load() {
  auto *widgetfactory = getPluginManager()->getInterface<WidgetFactoryInterface *>("/plugins/infrastructure/qpropertywidgetfactory");
  sTestGui = new PropertiesTestGui(widgetfactory);
  sTestGui->setWindowTitle("TestPlugin");

  QLabel *debugLabel = new QLabel();
  debugLabel->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);

  DebugLabelListener *listener = new DebugLabelListener(debugLabel);
  sTestGui->getAmbassador()->registerPropertyChangeListener(listener);

  dockwindow = new DockWindow;
  dockwindow->addWidget(debugLabel, Qt::BottomDockWidgetArea);

  menu = new QMenu("TestPlugin");
  QAction *dummyAction = new QAction(QObject::tr("&DummyAction"), menu);
  menu->addAction(dummyAction);
}

void TestPlugin::unload() {
  delete sTestGui;
}

LibFramework::InterfaceContainer* TestPlugin::createInterfaces() {
  return new LibFramework::InterfaceContainer(this);
}
