cmake_minimum_required(VERSION 3.22)
project(kadi-studio)

# line from the site
add_compile_definitions(QT_DISABLE_DEPRECATED_BEFORE=0x050F00)

set(CMAKE_CXX_STANDARD 20)

# scan header files and invoke MOC from Qt automatically
set(CMAKE_AUTOMOC ON)

# set(BUILD_TESTING OFF)

set(KADISTUDIO_COMPILE_PLUGINS ON)

# application plugins
# set(KADISTUDIO_PLUGIN_testplugin ON)
set(KADISTUDIO_PLUGIN_tooldialog ON)
set(KADISTUDIO_PLUGIN_workfloweditor ON)
set(KADISTUDIO_PLUGIN_workflowexecution ON)
set(KADISTUDIO_PLUGIN_workflowinteraction ON)

# infrastructure plugins
set(KADISTUDIO_PLUGIN_logdialog ON)
set(KADISTUDIO_PLUGIN_localfiledialog ON)
set(KADISTUDIO_PLUGIN_filedialogchooser ON)
set(KADISTUDIO_PLUGIN_registertooldialog ON)
set(KADISTUDIO_PLUGIN_dialogchooser ON)
set(KADISTUDIO_PLUGIN_custombuttondialog ON)
set(KADISTUDIO_PLUGIN_qpropertywidgetfactory ON)
set(KADISTUDIO_PLUGIN_processmanager ON)
set(KADISTUDIO_PLUGIN_kadifiledialog ON)
set(KADISTUDIO_PLUGIN_kadiconfig ON)
set(KADISTUDIO_PLUGIN_kadiintegration ON)

#
# sets the paths for libraries and binary files.
#
set(COMMON_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin)
set(COMMON_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)
link_directories(${PROJECT_BINARY_DIR}/lib)

set(KADISTUDIO_FRAMEWORK kadistudio_framework)

include_directories(lib/)
add_subdirectory(lib)

add_subdirectory(src)
add_subdirectory(plugins)

option(KADISTUDIO_BUILD_TESTING "enable unit tests" OFF)

if(KADISTUDIO_BUILD_TESTING)
  include(CTest)

  include(CheckIncludeFiles)
  include(CheckTypeSize)
  include(CheckLibraryExists)
  include(CheckSymbolExists)
  include(TestBigEndian)
  add_subdirectory(test)
endif(KADISTUDIO_BUILD_TESTING)
