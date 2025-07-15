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

#include <framework/pluginframework/pluginclientinterface.h>
#include "kadiinstance.h"


/**
 * @brief      Provides an interface to retrieve and edit Kadi instances.
 * @ingroup    kadiconfig
 */
class KadiConfigInterface : public LibFramework::PluginClientInterface {

public:
  ~KadiConfigInterface() override = default;

  virtual void showDialog() = 0;

  virtual QList<KadiInstance> getAllInstances() = 0;

  virtual KadiInstance getDefaultInstance() = 0;

};
