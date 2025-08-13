set(SRCS
  src/interaction.cpp
  interactionplugin.cpp
)

add_library(kadistudio_processmanager_interaction SHARED
  ${SRCS}
)

target_link_libraries(kadistudio_processmanager_interaction ${KADISTUDIO_FRAMEWORK} ${Qt6Widgets_LIBRARIES})

string(REPLACE ${PROJECT_SOURCE_DIR} "" outputdestination ${CMAKE_CURRENT_SOURCE_DIR})

set(outputdirectory ${COMMON_LIBRARY_OUTPUT_DIRECTORY}${outputdestination})

set_target_properties(kadistudio_processmanager_interaction PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${outputdirectory})
