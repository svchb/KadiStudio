/* Copyright 2022 Karlsruhe Institute of Technology
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

namespace LibFramework {
  class PluginInterface;
}

#ifdef Q_OS_WIN
#define PLUGIN_EXPORT __declspec(dllexport)
#else
#define PLUGIN_EXPORT
#endif

#define METADATA_MAGIC "studio-metadata "
#define PLUGIN_VERSION 1
#define PLUGIN_VERSION_TO_STRING_a(version) #version
#define PLUGIN_VERSION_TO_STRING_b(version) PLUGIN_VERSION_TO_STRING_a(version)
#define PLUGIN_VERSION_STRING PLUGIN_VERSION_TO_STRING_b(PLUGIN_VERSION)

//key is not a valid identifier in every case
#define PLUGIN_METADATA_STRING(varname, key, string) \
  const char *varname; \
  { \
    const char *metadata = METADATA_MAGIC #key " " string; \
    const int offset = sizeof(METADATA_MAGIC #key " ") - 1; \
    varname = metadata + offset; \
  } \

#define PLUGIN_INSTANCE(SPECIALIZED_TYPE) \
  extern "C" { \
  PLUGIN_EXPORT long getVersion() { \
    PLUGIN_METADATA_STRING(version, version, PLUGIN_VERSION_STRING) \
    (void) version; \
    return PLUGIN_VERSION; \
  } \
  PLUGIN_EXPORT const char* getClassName() { \
    PLUGIN_METADATA_STRING(className, classname, #SPECIALIZED_TYPE) \
    return className; \
  } \
  PLUGIN_EXPORT LibFramework::PluginInterface* createInstance() { \
    return new SPECIALIZED_TYPE(); \
  } \
  }

#define PLUGIN_AUTHORS(args ...) \
  extern "C" { \
  PLUGIN_EXPORT const char* getAuthors() { \
    PLUGIN_METADATA_STRING(authors, authors, #args) \
    return authors; \
  } \
  }

#define PLUGIN_NAME(NAME) \
  extern "C" { \
  PLUGIN_EXPORT const char* getName() { \
    PLUGIN_METADATA_STRING(name, name, #NAME) \
    return name; \
  } \
  }

#define PLUGIN_DESCRIPTION(DESCRIPTION) \
  extern "C" { \
  PLUGIN_EXPORT const char* getDescription() { \
    PLUGIN_METADATA_STRING(description, description, #DESCRIPTION) \
    return description; \
  } \
  }

#define PLUGIN_ICON(ICON) \
  extern "C" { \
  PLUGIN_EXPORT const char* getIcon() { \
    PLUGIN_METADATA_STRING(icon, icon, #ICON) \
    return icon; \
  } \
  }

#define PLUGIN_HELP(HELP) \
  extern "C" { \
  PLUGIN_EXPORT const char* getHelp() { \
    PLUGIN_METADATA_STRING(help, help, #HELP) \
    return help; \
  } \
  }

#define PLUGIN_NAMESPACE(NAMESPACE) \
  extern "C" { \
  PLUGIN_EXPORT const char* getNamespace() { \
    PLUGIN_METADATA_STRING(pluginNamespace, namespace, #NAMESPACE) \
    return pluginNamespace; \
  } \
  }

#define PLUGIN_REQUIRED_NAMESPACES(args ...) \
  extern "C" { \
  PLUGIN_EXPORT const char* getRequiredNamespaces() { \
    PLUGIN_METADATA_STRING(requiredNamespaces, required-namespaces, #args) \
    return requiredNamespaces; \
  } \
  }
