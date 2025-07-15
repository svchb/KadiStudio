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

#include "plugins/application/workfloweditor/config.h"
#include "sourcenode.h"

SourceNode::SourceNode() : WorkflowNode() {
  setNodeStyle(EditorConfig::NODE_STYLE);
}

bool SourceNode::captionVisible() const {
  return false; // false for all SourceNodes
}

unsigned int SourceNode::nPorts(PortType portType) const {
  return portType == PortType::In ? 0u : 1u;
}

QJsonObject SourceNode::save() const {
  QJsonObject modelJson = WorkflowNode::save();

  QString output_value = "";
  if (value && !value->getValue().isEmpty()) {
    output_value = value->getValue();
  } else {
    output_value = getDefaultValue();
  }
  modelJson["value"] = output_value;
  return modelJson;
}
