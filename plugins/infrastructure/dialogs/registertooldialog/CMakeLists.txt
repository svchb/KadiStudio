set(SRCS
  registertooldialogplugin.cpp
  src/registertooldialog.cpp
  src/searchdirectorywidget.cpp
  src/searchdirectorydialog.cpp
)

add_library(kadistudio_registertooldialog SHARED ${SRCS})

find_package(Qt5 COMPONENTS Concurrent REQUIRED)

target_link_libraries(kadistudio_registertooldialog ${KADISTUDIO_FRAMEWORK} Qt5::Widgets Qt5::Concurrent Qt5::Xml)

string(REPLACE ${PROJECT_SOURCE_DIR} "" outputdestination ${CMAKE_CURRENT_SOURCE_DIR})

set(outputdirectory ${COMMON_LIBRARY_OUTPUT_DIRECTORY}${outputdestination})

set_target_properties(kadistudio_registertooldialog PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${outputdirectory})
