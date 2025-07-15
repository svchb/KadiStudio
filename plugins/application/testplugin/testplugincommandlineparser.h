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

#include <framework/commandlineparser/commandlineparserapplicationinterface.h>

/**
 * @brief      Commandlineparser for the test plugin.
 * @ingroup    testplugin
 */
class TestPluginCommandlineParser : public CommandlineParserApplicationInterface {

  private:

    argument_t arguments[2] = {
      { NULL,   ' ', PARAM_OPTIONAL, "", NULL, "An input which name is printed on the commandline when the plugin is started", PARAM_FILEIN, &filename },
      { "test", 't', PARAM_OPTIONAL, "", NULL, "blubb", PARAM_LONG, &test },
    };

  public:

    virtual ~TestPluginCommandlineParser() {
    }

  protected:

    TestPluginCommandlineParser() : CommandlineParserApplicationInterface(ARGUMENT(arguments)) {
      filename = NULL;
    }

    toolparam_t getToolDescription(const std::string& pluginname) {
      description = "Description text of " + pluginname + "\n";
      example     = "Example text of " + pluginname;

      toolparam_t tool = {
        description.c_str(),
        example.c_str(),
        arguments,
        false
      };

      return tool;
    }

    char *filename;
    long test;

};
