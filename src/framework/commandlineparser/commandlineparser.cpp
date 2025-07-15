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

#include <QRegularExpression>

#include <pluginframework/pluginmanager.h>
#include <pluginframework/plugininfo/plugininfo.h>

#include "wrapper.h"

extern "C" {
  #include "parameter.h"
}

#include "commandlineparserapplicationinterface.h"

#include "commandlineparser.h"

#ifdef Q_OS_WIN
const char *libraryextension = ".dll";
#else
const char *libraryextension = ".so";
#endif


CommandLineParser::CommandLineParser() {
  this->pluginmanager = LibFramework::PluginManager::getInstance();
}

CommandLineParser::~CommandLineParser() {
}

void CommandLineParser::setArguments(int argc, char* argv[]) {
  arguments = std::vector<std::string>(argv, argv + argc);
}

bool CommandLineParser::autostartPlugins() {
  bool suc = false;
  if (arguments.size() > 1) {
    QString requestedpluginname = QString(arguments[1].c_str()).remove(QRegularExpression("^\\-+"));
    std::vector<const LibFramework::PluginInfo*> plugininfos = pluginmanager->getPluginInfos();

    for (const LibFramework::PluginInfo *plugininfo : plugininfos) {
      const std::string &namespacepath = plugininfo->getNamespace();
      if (QString::fromStdString(namespacepath).startsWith("/plugins/application")) {
        QString filename = QString::fromStdString(plugininfo->getFileName());
        QString pluginname = filename.remove("libkadistudio_").remove(libraryextension);
        if (pluginname == requestedpluginname) {
          if (pluginmanager->load(namespacepath)) {
            CommandlineParserApplicationInterface *parser = pluginmanager->getInterface<CommandlineParserApplicationInterface*>(namespacepath);
            if (parser) {
              std::vector<char*> argv = getPluginArguments();
              parser->parseArguments(requestedpluginname.toStdString(), argv);
            }
            suc = pluginmanager->run(namespacepath);
          } else {
            std::cerr << "Could not load requested plugin " << requestedpluginname.toStdString() << "." << std::endl;
            suc = false;
          }
          break;
        }
      }
    }
    if (!suc) {
      std::cerr << "Could not find requested plugin " << requestedpluginname.toStdString() << "." << std::endl;
      showHelp(false);
    }
  }
  return suc;
}

std::vector<char*> CommandLineParser::getPluginArguments() {
  std::vector<char*> argv;
  std::string program = arguments[0] + ' ' + arguments[1];
  argv.push_back(Strdup(program.c_str()));
  std::transform(std::begin(arguments) + 2, std::end(arguments), std::back_inserter(argv), [argv] (const std::string& argument) {
    return Strdup(argument.c_str());
  });
  return argv;
}

int CommandLineParser::evaluateHelp() {
  size_t argc = arguments.size() - 1;
  std::string help_argument = arguments[argc];
  if (argc == 0) {
    return 1;
  } else if (argc == 1) {
    if (help_argument == "-h" || help_argument == "--help" || help_argument == "--helpall" || help_argument == "--xmlhelp") {
      showHelp(help_argument == "--xmlhelp");
      return 0;
    }
  } else if (argc > 1) {
    if (help_argument == "-h" || help_argument == "--help" || help_argument == "--helpall" || help_argument == "--xmlhelp") {
      QString plugin_argument = QString(arguments[1].c_str()).remove("--");
      std::vector<const LibFramework::PluginInfo*> plugininfos = pluginmanager->getPluginInfos();

      for (const LibFramework::PluginInfo *plugininfo : plugininfos) {
        QString filename = QString::fromStdString(plugininfo->getFileName());
        QString pluginname = filename.remove("libkadistudio_").remove(libraryextension);

        if (plugin_argument == pluginname) {
          std::string namespacepath = plugininfo->getNamespace();
          CommandlineParserApplicationInterface *parser = pluginmanager->getInterface<CommandlineParserApplicationInterface*>(namespacepath);
          if (parser) {
            if (help_argument != "--xmlhelp") {
              parser->printUsage(arguments.front(), pluginname.toStdString());
            } else {
              std::vector<char*> argv = getPluginArguments();
              parser->parseArguments(pluginname.toStdString(), argv);
            }
            return 0;
          }
          std::cerr << "plugin " << pluginname.toStdString() << " has not implemented the CommandlineParserApplicationInterface" << std::endl
                    << "No help text is available so far." << std::endl;
          return -1;
        }
      }
    }
  }
  return -2;
}

void CommandLineParser::showHelp(bool xml) {
  size_t plugincount = 0;
  std::vector<const LibFramework::PluginInfo*> plugininfos = pluginmanager->getPluginInfos();

  for (const LibFramework::PluginInfo *plugininfo : plugininfos) {
    const std::string &namespacepath = plugininfo->getNamespace();
    if (QString::fromStdString(namespacepath).startsWith("/plugins/application")) {
      plugincount++;
    }
  }

  std::vector<argument_t> args;
  long dummy_parameter = false;

  for (const LibFramework::PluginInfo *plugininfo : plugininfos) {
    const std::string &namespacepath = plugininfo->getNamespace();
    if (QString::fromStdString(namespacepath).startsWith("/plugins/application")) {
      QString filename = QString::fromStdString(plugininfo->getFileName());
      QString pluginname = filename.remove("libkadistudio_").remove(libraryextension);
      const char *name = Strdup(pluginname.toStdString().c_str());
      const char *description = Strdup(plugininfo->getDescription().c_str());
      args.push_back(
        {
        name,
        ' ',
        PARAM_OPTIONAL,
        "",    // andparams
        NULL,  // interval
        description,
        PARAM_FLAG,
        &dummy_parameter
      });
    }
  }

  const char *name = arguments[0].c_str();

  std::string description = "This is the desktop application of the Kadi4Mat eco system.";
  std::string example     = "e.g.: " + arguments[0] + " --infileeditor arg1 arg2\n"
                            "This will start " + arguments[0] + " directly with the plugin \"infileeditor\" and hand over the parameters \"arg1\" and \"arg2\".\n"
                            "\n"
                            "e.g.: " + arguments[0] + " --infileeditor --help\n"
                            "This will show the help for the plugin \"infileeditor\".";

  const toolparam_t studio = {
    description.c_str(),
    example.c_str(),
    args.data(),
    false
  };


  if (xml) {
    const char *xmlhelp = "--xmlhelp";
    char *argv[] = { (char*)(name), (char*)xmlhelp };
    getParams(2, argv, studio, plugincount);
  } else {
    printUsage(name, studio, plugincount);
  }
}
