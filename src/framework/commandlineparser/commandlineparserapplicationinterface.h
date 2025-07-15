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

#include <string>

#include <framework/pluginframework/pluginclientinterface.h>

#define REAL double
extern "C" {
  #include "parameter.h"
}

/**
 * @brief      Interface for commandlineparsers of application
 *             plugins.
 * @ingroup    commandlineparser
 */
class CommandlineParserApplicationInterface : public LibFramework::PluginClientInterface {

  public:

    virtual ~CommandlineParserApplicationInterface() {
    }

    void parseArguments(const std::string& pluginname, std::vector<char*>& argv) {
      toolparam_t tool = getToolDescription(pluginname);
      int argc = argv.size();
      getParams(argc, &argv[0], tool, toolargc);
    }

    void printUsage(const std::string& argvzero, const std::string& pluginname) {
      toolparam_t tool = getToolDescription(pluginname);
      std::string plugin = argvzero + " --" + pluginname;
      ::printUsage(plugin.c_str(), tool, toolargc);
    }

  protected:

    CommandlineParserApplicationInterface(int toolargc) : toolargc(toolargc) {
    }

    virtual toolparam_t getToolDescription(const std::string& pluginname) = 0;

    std::string description;
    std::string example;

  private:

    int toolargc;

};
