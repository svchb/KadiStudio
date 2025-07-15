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

#include <QStringList>
#include <QtXml/QtXml>
#include <QIODevice>

#include "toolcache.h"
#include "toolxmldata.h"

ToolXMLData::ToolXMLData()
    : tooldescription() {
}

ToolXMLData::~ToolXMLData() {
}

bool ToolXMLData::createFromXML(const QString& xmlstring, const QString& command) {
  QDomDocument doc;

  if (!doc.setContent(xmlstring)) {
    qWarning() << "Error parsing XML tool description: " << xmlstring;
    return false;
  }

  QDomElement root = doc.documentElement();

  if (root.tagName() == "program" || root.tagName() == "env") {
    QString name = root.attribute("name", "");
    tooldescription.setName(name);

    tooldescription.setCommand(command);
    tooldescription.setShortName(QFileInfo(name).fileName());

    tooldescription.setDescription(root.attribute("description", ""));
    tooldescription.setExample(root.attribute("example", ""));
    if (root.hasAttribute("version")) {
      tooldescription.setVersion(root.attribute("version", ""));
    }
    tooldescription.setEnv((root.tagName() == "env"));
  } else {
    return false;
  }

  QDomNode n = root.firstChild();

  while (!n.isNull()) {
    QDomElement e = n.toElement();

    if (not e.isNull() && e.tagName() == "param") {
      ToolParameter parameter = ToolParameter(e);
      tooldescription.addParameter(parameter);
    }

    n = n.nextSibling();
  }

  tooldescription.removeHelp();

  return true;
}

bool ToolXMLData::createXML(const QString &toolidentificationstring) {
  tooldescription = ToolDescription();
  QProcess toolprocess;

  QStringList args = QProcess::splitCommand(toolidentificationstring);
  args.push_back("--xmlhelp");
  QString programm = args[0];
  args.pop_front();
  toolprocess.start(programm, args, QIODevice::ReadWrite); // in qt6, https://doc.qt.io/qt-6/qprocess.html#startCommand can be used

  if (not toolprocess.waitForFinished()) {
    qWarning() << "Could not retrieve XML tool description: " << programm << " " << args << " timed out";
    return false;
  }

  if (toolprocess.exitCode() != 0) {
    qWarning() << "Could not retrieve XML tool description: " << programm << " " << args << " returned exit code "
               << toolprocess.exitCode();
    return false;
  }

  QByteArray stdoutput = toolprocess.readAllStandardOutput();
  toolprocess.close();

  return createFromXML(stdoutput, toolidentificationstring);
}

bool ToolXMLData::createToolDescription(const QString& command) {
  QString toolidentificationstring;

  ToolDescription *cachedDescription = toolcache.get(command);
  if (cachedDescription) {
    tooldescription = *cachedDescription;
  } else if (createXML(command)) {
    // give ownership of the new ToolDescription pointer to the cache
    toolcache.insert(toolidentificationstring,
                                     std::make_unique<ToolDescription>(tooldescription));
  } else {
    return false;
  }
  return true;
}

const ToolDescription& ToolXMLData::Description() const {
  return tooldescription;
}

void ToolXMLData::resetCache() {
  toolcache.reset();
}
