set(SRCS
  processmanagerplugin.cpp
  src/processmanager.cpp
)

add_library(kadistudio_processmanager SHARED
  ${SRCS}
)

target_link_libraries(kadistudio_processmanager ${KADISTUDIO_FRAMEWORK} ${Qt5Widgets_LIBRARIES})

string(REPLACE ${PROJECT_SOURCE_DIR} "" outputdestination ${CMAKE_CURRENT_SOURCE_DIR})

set(outputdirectory ${COMMON_LIBRARY_OUTPUT_DIRECTORY}${outputdestination})

set_target_properties(kadistudio_processmanager PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${outputdirectory})
