set(SRCS
  src/workflow.cpp
  workflowplugin.cpp
)

add_library(kadistudio_processmanager_workflow SHARED
  ${SRCS}
)

target_link_libraries(kadistudio_processmanager_workflow ${KADISTUDIO_FRAMEWORK} ${Qt6Widgets_LIBRARIES})

string(REPLACE ${PROJECT_SOURCE_DIR} "" outputdestination ${CMAKE_CURRENT_SOURCE_DIR})

set(outputdirectory ${COMMON_LIBRARY_OUTPUT_DIRECTORY}${outputdestination})

set_target_properties(kadistudio_processmanager_workflow PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${outputdirectory})
