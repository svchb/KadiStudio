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

#include "application.h"

int main(int argc, char** argv) {

  // Creation of the Application instance
  QCoreApplication::setAttribute(Qt::AA_ShareOpenGLContexts);
  Application app("kadistudio", argc, argv);
  app.setApplicationDisplayName("Kadi Studio");

  int ret = app.evaluateHelp(argc, argv);

  // show help message and exit
  if (ret <= 0) {
    return ret;
  }

  app.showMainWindow();

  // Go
  return app.exec();
}
