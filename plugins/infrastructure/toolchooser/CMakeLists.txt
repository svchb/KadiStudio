include_directories(
)

set(SRCS
  toolchooserplugin.cpp

  src/toolchooserwidget.cpp
  src/infotabwidget.cpp

  src/tooldata/tooldescription.cpp
  src/tooldata/toolxmldata.cpp
  src/tooldata/toolcache.cpp
  src/tooldata/toolcache.h
)

add_library(kadistudio_toolchooser SHARED
  ${SRCS}
)

target_link_libraries(kadistudio_toolchooser
  ${KADISTUDIO_FRAMEWORK}
  Qt6::Xml
  Qt6::Widgets
)

string(REPLACE ${PROJECT_SOURCE_DIR} "" outputdestination ${CMAKE_CURRENT_SOURCE_DIR})

set(outputdirectory ${COMMON_LIBRARY_OUTPUT_DIRECTORY}${outputdestination})

set_target_properties(kadistudio_toolchooser PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${outputdirectory})
