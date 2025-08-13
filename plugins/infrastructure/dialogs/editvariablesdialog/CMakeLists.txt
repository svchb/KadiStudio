set(SRCS
  editvariablesdialogplugin.cpp
  src/editvariablesdialog.cpp
)

add_library(kadistudio_editvariabledialogplugin SHARED ${SRCS})

target_link_libraries(kadistudio_editvariabledialogplugin ${KADISTUDIO_FRAMEWORK} Qt6::Widgets)

string(REPLACE ${PROJECT_SOURCE_DIR} "" outputdestination ${CMAKE_CURRENT_SOURCE_DIR})

set(outputdirectory ${COMMON_LIBRARY_OUTPUT_DIRECTORY}${outputdestination})

set_target_properties(kadistudio_editvariabledialogplugin PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${outputdirectory})
