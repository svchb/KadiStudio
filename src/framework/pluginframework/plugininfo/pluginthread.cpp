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

#include <functional>

#include "pluginthread.h"
#include "../plugininterface.h"

namespace LibFramework {

PluginThread::PluginThread(PluginInterface* const plugininterface)
    : threadstarted(false),
      exit(false),
      startplugin(false),
      processloop(false) {
  this->plugininterface = plugininterface;
  thread = std::thread {&PluginThread::run, this};
  std::unique_lock<std::mutex> lock(mtx_started);
  condvar_started.wait(lock, [&] () {
    return threadstarted == true;
  });
}

PluginThread::~PluginThread() {
  {
    std::lock_guard<std::mutex> lock(mtx);
    exit = true;
    processloop = true;
  }
  condvar.notify_all();
  thread.join();
}

void PluginThread::start() {
  {
    std::lock_guard<std::mutex> lock(mtx);
    startplugin = true;
    processloop = true;
  }
  condvar.notify_all();
}

void PluginThread::run() {
  std::unique_lock<std::mutex> lock(mtx);
  mtx_started.lock();
  threadstarted = true;
  mtx_started.unlock();
  condvar_started.notify_all();

  while (true) {
    condvar.wait(lock, [&] () {
      return processloop == true;
    });

    processloop = false;
    changeState();

    if (exit) {
      break;
    }
  }
}

void PluginThread::changeState() {
  if (startplugin) {
    plugininterface->run();
    startplugin = false;
  }
}

}
