/* Copyright 2022 Karlsruhe Institute of Technology
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

#include <QDebug>
#include <QListWidget>
#include <QListWidgetItem>
#include <QHBoxLayout>
#include <QLabel>

#include <pluginframework/plugininfo/plugininfo.h>
#include <pluginframework/pluginchooser/pluginchooser.h>
#include <pluginframework/pluginmanager.h>

#include "aboutdialog.h"


AboutStudioDialog::AboutStudioDialog(const QString& callernamespace, QWidget* parent) : QDialog(parent) {
  setWindowTitle("About Kadi Studio");
  setMinimumWidth(600);
  setMinimumHeight(400);

  QHBoxLayout *dialogLayout = new QHBoxLayout();
  setLayout(dialogLayout);

  listwidget = new QListWidget();
  listwidget->setMinimumWidth(200);
  listwidget->setFocusPolicy(Qt::StrongFocus);
  listwidget->setSelectionMode(QAbstractItemView::SingleSelection);
  listwidget->setSelectionBehavior(QAbstractItemView::SelectItems);
  // listwidget->setEditTriggers(QAbstractItemView::AllEditTriggers);
  dialogLayout->addWidget(listwidget);

  LibFramework::PluginManagerInterface *pluginmanager = LibFramework::PluginManager::getInstance();
  std::vector<const LibFramework::Plugin*> plugins = pluginmanager->getPlugins();

  for (const LibFramework::Plugin *plugin : plugins) {
    const LibFramework::PluginInfo *plugininfo = plugin->getPluginInfo();
    QString icon = QString::fromStdString(plugininfo->getIcon());
    QString pluginname = QString::fromStdString(plugininfo->getName());

    ListWidgetItem *item = new ListWidgetItem(plugin);
    item->setIcon(QIcon(icon));
    item->setSizeHint(QSize(150, 51));

    item->setText(pluginname);

    listwidget->addItem(item);
    item->setSelected(false);
  }

  basicInformationLbl = new QLabel();
  basicInformationLbl->setTextFormat(Qt::RichText);
  basicInformationLbl->setWordWrap(true);
  setAboutText();
  dialogLayout->addWidget(basicInformationLbl, Qt::AlignJustify|Qt::AlignTop);

  connect(listwidget, SIGNAL(currentItemChanged(QListWidgetItem *, QListWidgetItem *)), this, SLOT(setSelected(QListWidgetItem *)));
  connect(listwidget, SIGNAL(itemSelectionChanged()), this, SLOT(itemSelectionChanged()));
  connect(listwidget, SIGNAL(currentRowChanged(int)), this, SLOT(currentRowChanged(int)));
  connect(listwidget, SIGNAL(itemClicked(QListWidgetItem *)), this, SLOT(itemClicked(QListWidgetItem *)));

  previousselected = nullptr;
  changed = -1;

  if (!callernamespace.isEmpty()) {
    int row = 0;
    for (const LibFramework::Plugin *plugin : plugins) {
      const LibFramework::PluginInfo *plugininfo = plugin->getPluginInfo();
      QString pluginnamespace = QString::fromStdString(plugininfo->getNamespace());

      if (callernamespace == pluginnamespace) {
        listwidget->setCurrentRow(row);
        setPluginDescription(listwidget->currentItem());
        break;
      }
      row++;
    }
  }
}

void AboutStudioDialog::setAboutText() {
  basicInformationLbl->setText(tr("<h1>Kadi Studio</h1>"
                                  "<p>This software provides a plugin-based graphical frontend and is part of the Kadi ecosystem.</p>"
                                  "<br/>"
                                  "<p>Copyright (c) 2005-2018 by University of Applied Sciences Karlsruhe.</p>"
                                  "<p>Copyright (c) 2018-2021 Institute for Applied Materials - Computational Materials Science (IAM-CMS), Karlsruhe Institute of Technology.</p>"
                                  "<p>Copyright (c) 2022 Institute for Applied Materials - Microstructure Modelling and Simulation (IAM-MMS), Karlsruhe Institute of Technology.</p>"
                                  "<p>Copyright (c) 2023- Institute for Nanotechnologies, Karlsruhe Institute of Technology.</p>"
                                  "<p>All Rights Reserved.</p>"));
}

void AboutStudioDialog::setPluginDescription(QListWidgetItem *selected) {
  ListWidgetItem *item = reinterpret_cast<ListWidgetItem*>(selected);

  const LibFramework::PluginInfo *plugininfo = item->getPlugin()->getPluginInfo();
  std::set<std::string> pluginauthors = plugininfo->getAuthors();
  std::set<std::string>::iterator it;
  QString authors;
  for (it = pluginauthors.begin(); it != pluginauthors.end(); ++it) {
    if (it != pluginauthors.begin()) {
      authors += ", ";
    }
    const std::string &author = (*it);
    authors += QString::fromStdString(author);
  }
  // QPixmap pixmap;
  // pixmap.load(":/studio/framework/pixmaps/kadi.png");
  // basicInformationLbl->setPixmap(pixmap);
  // basicInformationLbl->setStyleSheet("background-image:url(:/studio/framework/pixmaps/kadi.png)");

  basicInformationLbl->setText(tr("<h1>%1</h1>"
                            "<br/>"
                            "<table width=\"90%\">"
                            "<tr><td><b>Description:</b></td><td> </td><td>%2</td></tr>"
                            "<tr><td><b>Author(s):  </b></td><td> </td><td>%3</td></tr>"
                            "<tr><td><b>Namespace:  </b></td><td> </td><td>%4</td></tr>"
                            "<tr><td><b>Filename:   </b></td><td> </td><td>%5</td></tr>"
                            "<tr><td><b>Path:       </b></td><td> </td><td>%6</td></tr>"
                            "</table>"
                            "<br/>"
                            "<br/>"
               ).arg(QString::fromStdString(plugininfo->getName()))
                .arg(QString::fromStdString(plugininfo->getDescription()))
                .arg(authors)
                .arg(QString::fromStdString(plugininfo->getNamespace()))
                .arg(QString::fromStdString(plugininfo->getFileName()))
                .arg(QString::fromStdString(plugininfo->getFilePath()))
  );
}

void AboutStudioDialog::currentRowChanged(int currentRow) {
  // qDebug() << "AboutStudioDialog::currentRowChanged() index " << currentRow;
  if (changed == 5) {
    changed = 0; // might be key
    previousselected = listwidget->item(currentRow);
  } else {
    // is mouse
  }
}

void AboutStudioDialog::itemSelectionChanged() {
  // qDebug() << "AboutStudioDialog::itemSelectionChanged() index " << listwidget->currentRow();
  if (changed == 0) {
    changed = 5;  // might be key
  }
}

void AboutStudioDialog::setSelected(QListWidgetItem* selected) {
  // qDebug() << "AboutStudioDialog::setSelected 1row [" << listwidget->row(selected) << "] == " << selected->text();
  if (changed != -1) setPluginDescription(selected);
}

void AboutStudioDialog::itemClicked(QListWidgetItem* clicked) {
  // qDebug("itemClicked") ;
  // qDebug() << "selected " << previousselected;
  // qDebug() << "changed " << changed;

  if (previousselected == nullptr) {
    setPluginDescription(clicked);
  } else if (changed == 0) {
    if (previousselected == clicked) {
      previousselected = nullptr;
      listwidget->clearSelection();
      setAboutText();
      return;
    }
  }

  previousselected = clicked;
  changed = 0;
}
