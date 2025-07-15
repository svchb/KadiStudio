set(SRCS
  loggerplugin.cpp
  src/logger.cpp
)

add_library(kadistudio_logger SHARED ${SRCS})

target_link_libraries(kadistudio_logger ${KADISTUDIO_FRAMEWORK} ${Qt5Widgets_LIBRARIES})

string(REPLACE ${PROJECT_SOURCE_DIR} "" outputdestination ${CMAKE_CURRENT_SOURCE_DIR})

set(outputdirectory ${COMMON_LIBRARY_OUTPUT_DIRECTORY}${outputdestination})

set_target_properties(kadistudio_logger PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${outputdirectory})
