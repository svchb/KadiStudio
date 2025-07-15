set(SRCS
  kadifiledialogplugin.cpp
  src/kadifiledialog.cpp
)

add_library(kadistudio_kadifiledialog SHARED ${SRCS})

target_link_libraries(kadistudio_kadifiledialog ${KADISTUDIO_FRAMEWORK} Qt5::Widgets)

string(REPLACE ${PROJECT_SOURCE_DIR} "" outputdestination ${CMAKE_CURRENT_SOURCE_DIR})

set(outputdirectory ${COMMON_LIBRARY_OUTPUT_DIRECTORY}${outputdestination})

set_target_properties(kadistudio_kadifiledialog PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${outputdirectory})
