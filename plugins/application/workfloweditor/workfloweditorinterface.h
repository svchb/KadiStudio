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

#include <QString>

#include <framework/commandlineparser/commandlineparserapplicationinterface.h>

/**
 * @ingroup    workfloweditor
 * @brief      Interface for the workflow editor
 */
class WorkflowEditorInterface : public CommandlineParserApplicationInterface {

  public:
    virtual ~WorkflowEditorInterface() = default;

    virtual void openWorkflow(const QString& fileName) = 0;
    virtual char* getPathArgument() {
      return workflow_path;
    };

  protected:
    WorkflowEditorInterface() : CommandlineParserApplicationInterface(ARGUMENT(arguments)) {
      workflow_path = nullptr;
    }

    toolparam_t getToolDescription(const std::string& pluginname) {
      description = "\nWorkflow Editor Plugin\n";
      example     = "e.g.: kadistudio --" + pluginname + " myfolder/test.flow\n";

      toolparam_t tool = {
        description.c_str(),
        example.c_str(),
        arguments,
        false
      };

      return tool;
    }

    char *workflow_path;


  private:
    argument_t arguments[1] = {
      {
        NULL,
        ' ',
        PARAM_OPTIONAL,
        "",
        NULL,
        "Workflow file",
        PARAM_FILEIN,
        &workflow_path
      }
    };
};
