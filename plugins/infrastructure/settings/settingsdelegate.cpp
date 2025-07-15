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

#include "settingsdelegate.h"

#include <framework/menu/menudelegate.h>
#include <framework/pluginframework/plugininterface.h>
#include <framework/pluginframework/plugininfo/plugininfo.h>
#include <framework/pluginframework/pluginmanager.h>

#include <QApplication>
#include <QMenu>
#include <QSettings>

#include "gui/settingsgui.h"


SettingsDelegate::SettingsDelegate(WidgetFactoryInterface* wfi) : wfi(wfi) {
}

SettingsDelegate::~SettingsDelegate() {
  for (auto it = plugin_settings.begin(); it != plugin_settings.end(); ++it) {
    delete it->second;
  }
  plugin_settings.clear();
}

void SettingsDelegate::setPluginSetting(const QString& plugin_namespace,
                                        const QString& key,
                                        const QVariant& value) {
  QSettings *settings = getPluginSettings(plugin_namespace);
  settings->setValue(key, value);
  settings->sync();
}

QVariant SettingsDelegate::getPluginSetting(const QString& plugin_namespace,
                                            const QString& key,
                                            const QVariant& default_value) {
  QSettings *settings = getPluginSettings(plugin_namespace);

  if (settings->contains(key)) {
    return settings->value(key);
  } else {
    settings->setValue(key, default_value);
    settings->sync();
    return default_value;
  }
}

QSettings* SettingsDelegate::getPluginSettings(const QString& plugin_namespace) {
  auto it = plugin_settings.find(plugin_namespace);
  if (it == plugin_settings.end()) {
    QSettings *settings = new QSettings(qApp->applicationName(), plugin_namespace);
    plugin_settings[plugin_namespace] = settings;
    return settings;
  }

  return it->second;
}

void SettingsDelegate::addConfigMenu(const QString& plugin_namespace, QMenu* menu) {
  LibFramework::PluginManagerInterface *pluginmanager = LibFramework::PluginManager::getInstance();
  std::vector<const LibFramework::PluginInfo*> plugininfos = pluginmanager->getPluginInfos(plugin_namespace.toStdString());

  if (plugininfos.size() != 1) throw std::runtime_error("settings: namespaces are not allowed only pluginnames");

  const LibFramework::PluginInfo *plugininfo = plugininfos[0];
  QString pluginname = QString::fromStdString(plugininfo->getName());

  auto config_dialog_action = new QAction(QString(QObject::tr("Configure %1 ...").arg(pluginname)));
  config_dialog_action->setIcon(QIcon(":/studio/plugins/infrastructure/settings/icons/fa-sliders-h.svg"));

  QObject::connect(config_dialog_action, &QAction::triggered, [plugin_namespace, this]() {
    SettingsGui dialog(getPluginSettings(plugin_namespace), wfi);
    dialog.setWindowTitle(QObject::tr("Settings"));
    dialog.exec();
  });

  menu->addAction(config_dialog_action);
}

void SettingsDelegate::createConfigMenu(const QString& plugin_namespace) {
  auto menu = new QMenu(QObject::tr("Settings"));

  addConfigMenu(plugin_namespace, menu);

  MenuInterface *menuinterface = MenuDelegate::getInstance();
  menuinterface->addMenu(plugin_namespace, menu);
}
