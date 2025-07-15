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

class WorkflowInteractionInterface : public CommandlineParserApplicationInterface {

public:
  ~WorkflowInteractionInterface() override = default;

  virtual void setWorkflowId(unsigned int id) = 0;
  virtual void setWorkflowFile(QString fileName) = 0;

  char *getWorkflowFileName() const {
    return workflowFileName;
  }

protected:
  WorkflowInteractionInterface() : CommandlineParserApplicationInterface(ARGUMENT(arguments)) {
    workflowFileName = nullptr;
  }

  toolparam_t getToolDescription(const std::string& pluginname) override {
    description = "\nPlugin for interaction workflow execution\n";
    example     = "e.g.: kadistudio --" + pluginname + " workflow_file\n";

    toolparam_t tool = {
      description.c_str(),
      example.c_str(),
      arguments,
      false
    };

    return tool;
  }

  char *workflowFileName;


private:
  argument_t arguments[1] = {
    { nullptr, ' ', PARAM_OPTIONAL, "", nullptr, "Workflow file", PARAM_FILEIN, &workflowFileName }
  };
};
