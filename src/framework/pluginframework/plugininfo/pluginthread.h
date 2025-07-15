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

#include <mutex>
#include <condition_variable>
#include <thread>

namespace LibFramework {

class PluginInterface;

/**
 * @brief      The thread in which the plugin is executed.
 * @ingroup    framework
 */
class PluginThread {

  public:

    PluginThread(PluginInterface* plugininterface);
    PluginThread(const PluginThread& src) = delete;
    PluginThread& operator=(const PluginThread& rhs) = delete;
    ~PluginThread();

    void start();

  private:

    void changeState();
    void run();

    bool threadstarted;
    bool exit;
    std::mutex mtx;
    std::condition_variable condvar;
    std::thread thread;
    std::mutex mtx_started;
    std::condition_variable condvar_started;
    bool startplugin;
    bool processloop;
    PluginInterface *plugininterface;

};

}
