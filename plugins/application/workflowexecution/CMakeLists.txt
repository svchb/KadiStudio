set(SRCS
  workflowexecutionplugin.cpp
  WorkflowExecution.cpp
  workflowtable/StatusDelegate.cpp
  workflowtable/WorkflowTableModel.cpp
  WorkflowExecutionDelegate.cpp
  workflowtable/WorkflowTableSortFilter.cpp
)

add_library(kadistudio_workflowexecution SHARED
  ${SRCS}
)

target_include_directories(kadistudio_workflowexecution
  PRIVATE ${PROJECT_SOURCE_DIR}/plugins/infrastructure/workflows/processmanager/
)

target_link_libraries(kadistudio_workflowexecution
  ${KADISTUDIO_FRAMEWORK}
  Qt6::Xml
  ${Qt6Widgets_LIBRARIES}
)

string(REPLACE ${PROJECT_SOURCE_DIR} "" outputdestination ${CMAKE_CURRENT_SOURCE_DIR})

set(outputdirectory ${COMMON_LIBRARY_OUTPUT_DIRECTORY}${outputdestination})

set_target_properties(kadistudio_workflowexecution PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${outputdirectory})
