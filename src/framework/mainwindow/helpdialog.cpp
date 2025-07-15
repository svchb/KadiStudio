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

#include <iostream>
#include <QDebug>
#include <QUuid>
#include <QListWidget>
#include <QListWidgetItem>
#include <QHBoxLayout>
#include <QXmlStreamReader>

#include <QTextBrowser>
#include <QFile>

#include <pluginframework/plugininfo/plugininfo.h>
#include <pluginframework/pluginchooser/pluginchooser.h>
#include <pluginframework/pluginmanager.h>

#include "helpdialog.h"


HelpDialog::HelpDialog(const QString& callernamespace, QWidget* parent) : QDialog(parent) {
  setWindowTitle("Help Pages");
  setMinimumWidth(750);
  setMinimumHeight(500);

  QHBoxLayout *dialogLayout = new QHBoxLayout();
  setLayout(dialogLayout);

  listwidget = new QListWidget();
  listwidget->setMinimumWidth(175);
  listwidget->setFocusPolicy(Qt::StrongFocus);
  listwidget->setSelectionMode(QAbstractItemView::SingleSelection);
  listwidget->setSelectionBehavior(QAbstractItemView::SelectItems);
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

  content = new QTextBrowser();
  content->setAlignment(Qt::AlignTop|Qt::AlignLeft);
  content->setOpenExternalLinks(true);
  setDefaultHelpText();
  dialogLayout->addWidget(content, Qt::AlignJustify|Qt::AlignTop);

  connect(listwidget, SIGNAL(currentItemChanged(QListWidgetItem*,QListWidgetItem*)), this, SLOT(setSelected(QListWidgetItem*)));
  connect(listwidget, SIGNAL(itemSelectionChanged()), this, SLOT(itemSelectionChanged()));
  connect(listwidget, SIGNAL(currentRowChanged(int)), this, SLOT(currentRowChanged(int)));
  connect(listwidget, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(itemClicked(QListWidgetItem*)));

  previousselected = nullptr;
  changed = -1;

  if (!callernamespace.isEmpty()) {
    int row = 0;
    for (const LibFramework::Plugin *plugin : plugins) {
      const LibFramework::PluginInfo *plugininfo = plugin->getPluginInfo();
      QString pluginnamespace = QString::fromStdString(plugininfo->getNamespace());

      if (callernamespace == pluginnamespace) {
        listwidget->setCurrentRow(row);
        setPluginHelpText(listwidget->currentItem());
        break;
      }
      row++;
    }
  }
}

void HelpDialog::setDefaultHelpText() {
  content->setHtml(tr("<h1>Kadi Studio Help Pages</h1>"
                      "<p>This window gives you basic usability information about each Plugin.</p>"
                      "<p>Click on a Plugin from the list to get it's Help Page.</p>"));
}

typedef struct {
  int level;
  QString ref;
  QString title;
} tocentry;

void HelpDialog::setPluginHelpText(QListWidgetItem *selected) {
  ListWidgetItem *item = reinterpret_cast<ListWidgetItem*>(selected);

  QString text;

  const LibFramework::PluginInfo *plugininfo = item->getPlugin()->getPluginInfo();
  std::string helptxt = plugininfo->getHelp();
  if (helptxt.empty()) {
    text = "<br/>" + tr("Oh oh, there does not seem to be any help available for this plugin yet...");
  } else {

    QFile file(QString::fromStdString(helptxt));
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {

      QXmlStreamReader xmlReader(&file);

      QList<tocentry> toc;
      tocentry newentry;
      bool isheadline = false;

      xmlReader.readNext();
      while (not xmlReader.isEndDocument() && not xmlReader.hasError()) {
        xmlReader.readNext();

        if (xmlReader.isStartElement()) {
          QString starttag = "<" + xmlReader.name().toString();

          QXmlStreamAttributes attributes = xmlReader.attributes();

          if (xmlReader.name().toString() == "h2") {
            newentry.level = 2;
            newentry.ref = attributes.value("id").toString();
            if (newentry.ref.isEmpty()) {
              // create
              newentry.ref = QUuid::createUuid().toString();
            }
            starttag += QString(" id=\"%1\"").arg(newentry.ref);
            isheadline = true;
          }
          if (xmlReader.name().toString() == "h3") {
            newentry.level = 3;
            newentry.ref = attributes.value("id").toString();
            if (newentry.ref.isEmpty()) {
              // create
              newentry.ref = QUuid::createUuid().toString();
            }
            starttag += QString(" id=\"%1\"").arg(newentry.ref);
            isheadline = true;
          }


          for (auto attr : attributes) {
            if (attr.name() == "id") continue;
            QString attrstring = " " + attr.name() + "=\"" + attr.value() + "\"";
            starttag += attrstring;
          }
          starttag += ">";

          text += starttag;

        } else if (xmlReader.isEndElement()) {
          QString endtag = "</" + xmlReader.name() + ">";
          text += endtag;
        } else if (xmlReader.isCharacters()) {
          text += xmlReader.text();
          if (isheadline) {
            newentry.title = xmlReader.text().toString();
            toc.append(newentry);
            isheadline = false;
          }
        }
      }


      if (xmlReader.hasError()) {
        file.reset();
        QTextStream in(&file);
        text = in.readAll();
      } else {
        QString tocstring;
        tocstring = QString("<div class=\"toc_container\">"
                              "<h2 class=\"toc_title\">%1</h2>"
                              "<ul class=\"toc_list\" role=\"list\">\n").arg(tr("Contents"));

        int prevlevel = 2;
        for (auto toctoc : toc) {
          if (toctoc.level > prevlevel) {
            tocstring += "<ul class=\"toc_list\">";
          }
          tocstring += QString("<li class=\"toc_list\"><a href=\"#%1\">%2</a></li>").arg(toctoc.ref, toctoc.title);
          if (toctoc.level < prevlevel) {
            tocstring += "</ul>";
          }
          prevlevel = toctoc.level;
          qDebug() << toctoc.level << " " <<toctoc.ref << " " << toctoc.title;
        }

        tocstring += "</ul></div>";
        text = tocstring + text;

      }

      file.close();

      std::cout << content->toHtml().toStdString() << std::endl;
    } else {
      qDebug() << "Failed to open the resource file.";
      text = "<br/>" + tr("Failed to open the resource file.");
    }

  }

  content->setHtml(QString("<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.1 Transitional//EN\"\"http://www.w3.org/TR/html4/loose.dtd\">  <title>Pace3D documentation</title><html>"
            "<head>"
            "<style>"
            ".toc_container {"
              // "background: #f9f9f9 none repeat scroll 0 0;"
              // "border: 1px solid #aaa;"
              // "display: table;"
              // "font-size: 95%;"
              // "margin-bottom: 1em;"
              // "padding: 20px;"
              // "width: auto;"
            "}"
            ".toc_title {"
              // "font-weight: 700;"
              // "font-size: 195%;"
              // "text-align: center;"
            "}"
            ".toc_list {"
              "list-style: outside none none !important;"
              "underline: off;"
            "}"
            "</style>"
            "</head>%2"
            "<h1>%1</h1>%2<br/>%3").arg(QString::fromStdString(plugininfo->getName()),
                                        QString::fromStdString(plugininfo->getDescription()),
                                        text));
}

void HelpDialog::currentRowChanged(int currentRow) {
  if (changed == 5) {
    changed = 0; // might be key
    previousselected = listwidget->item(currentRow);
  } else {
    // is mouse
  }
}

void HelpDialog::itemSelectionChanged() {
  if (changed == 0) {
    changed = 5;  // might be key
  }
}

void HelpDialog::setSelected(QListWidgetItem* selected) {
  if (changed != -1) setPluginHelpText(selected);
}

void HelpDialog::itemClicked(QListWidgetItem* clicked) {
  if (previousselected == nullptr) {
    setPluginHelpText(clicked);
  } else if (changed == 0) {
    if (previousselected == clicked) {
      previousselected = nullptr;
      listwidget->clearSelection();
      setDefaultHelpText();
      return;
    }
  }

  previousselected = clicked;
  changed = 0;
}
