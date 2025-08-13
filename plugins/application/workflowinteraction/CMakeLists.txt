set(SRCS
  WorkflowInteractionPlugin.cpp
  WorkflowInteraction.cpp
  WorkflowInteractionDelegate.cpp
  properties/PropertyParser.cpp
  properties/jsonserialize.cpp
  widgets/WorkflowInfoWidget.cpp
  widgets/InteractionPageWidget.cpp
  widgets/InteractionWidget.cpp
)

set(RCCS workflowinteraction.qrc)
QT6_ADD_RESOURCES(RCC_SRCS ${RCCS})

add_library(kadistudio_workflowinteraction SHARED
  ${SRCS}
  ${RCC_SRCS}
)

target_include_directories(kadistudio_workflowinteraction
  PRIVATE ${PROJECT_SOURCE_DIR}/plugins/infrastructure/qpropertywidgetfactory/
  PRIVATE ${PROJECT_SOURCE_DIR}/plugins/infrastructure/workflows/processmanager/
  PRIVATE ${PROJECT_SOURCE_DIR}/plugins/infrastructure/dialogs/logdialog/
)

target_link_libraries(kadistudio_workflowinteraction
  ${KADISTUDIO_FRAMEWORK}
  ${Qt6Widgets_LIBRARIES}
  properties
)

string(REPLACE ${PROJECT_SOURCE_DIR} "" outputdestination ${CMAKE_CURRENT_SOURCE_DIR})

set(outputdirectory ${COMMON_LIBRARY_OUTPUT_DIRECTORY}${outputdestination})

set_target_properties(kadistudio_workflowinteraction PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${outputdirectory})
