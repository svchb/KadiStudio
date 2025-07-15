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

#include <framework/tab/tabdelegate.h>
#include <framework/menu/menudelegate.h>

#include "gui/toolstarterwidget.h"
#include "toolstarterplugin.h"

void ToolStarter::run() {
  TabInterface *tab = TabDelegate::getInstance();
  tab->addTab("/plugins/application/toolstarter", gui, "ToolStarter");

  MenuInterface *menu = MenuDelegate::getInstance();
  menu->addMenu("/plugins/application/toolstarter", gui->getMenu());
}

void ToolStarter::load() {
  gui = new ToolStarterWidget(getPluginManager());
}

void ToolStarter::unload() {
  delete gui;
}
