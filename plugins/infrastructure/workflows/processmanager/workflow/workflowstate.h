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

/**
 * @file       workflowstate.h
 * @ingroup    processmanager
 * @brief      An enum describing the states of a workflow.
 */

#pragma once

enum WorkflowState {
  READY,
  RUNNING,
  NEEDS_INTERACTION,
  CANCELLING,
  CANCELLED,
  ERROR,
  FINISHED
};
