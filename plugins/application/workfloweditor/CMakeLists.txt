find_package(Qt6 COMPONENTS Widgets Svg SvgWidgets REQUIRED)
find_package(Qt6PrintSupport REQUIRED)

set(SRCS
  autolayout.cpp
  workfloweditorplugin.cpp
  workfloweditor.cpp
  WorkFlowGraphModel.cpp
  workflowview.cpp
  nodes/models/toolnode.cpp
  nodes/models/notenode.cpp
  nodes/models/envnode.cpp
  nodes/models/executablenode.cpp
  nodes/models/control/ifnode.cpp
  nodes/models/control/variablenode.cpp
  nodes/models/control/variablejsonnode.cpp
  nodes/models/control/variablelistnode.cpp
  nodes/models/io/fileinputnode.cpp
  nodes/models/misc/formatstringnode.cpp
  nodes/models/sources/stringsourcenode.cpp
  nodes/models/sources/floatsourcenode.cpp
  nodes/models/sources/integersourcenode.cpp
  domain/tool.cpp
  nodes/models/sources/booleansourcenode.cpp
  nodes/models/io/fileoutputnode.cpp
  workflowscene.cpp
  nodes/models/user-io/userinputtextnode.cpp
  nodes/models/user-io/userinputfilenode.cpp
  nodes/models/user-io/userinputcropimagesnode.cpp
  nodes/models/user-io/userinputselectboundingboxnode.cpp
  nodes/models/user-io/userinputperiodictablenode.cpp
  nodes/models/user-io/userinputintegernode.cpp
  nodes/models/user-io/useroutputtextnode.cpp
  nodes/models/user-io/userinputfloatnode.cpp
  nodes/models/user-io/userinputboolnode.cpp
  nodes/models/user-io/userinputformnode.cpp
  nodes/models/control/loopnode.cpp
  nodes/models/user-io/userinputchoosenode.cpp
  nodes/models/user-io/userinputselectnode.cpp
  nodes/models/control/branchselectnode.cpp
  nodes/data/genericnodedata.cpp
  nodes/models/workflownode.cpp
  nodes/models/sources/sourcenode.cpp
  widgets/executionprofilechooser.cpp
  widgets/inserttooldialog.cpp
  domain/executionprofile.cpp
)
if(WEBVIEW_SUPPORT_ENABLED)
  list(APPEND SRCS
    nodes/models/user-io/useroutputwebviewnode.cpp
  )
endif()

add_subdirectory(thirdparty)

set(RCCS workfloweditor.qrc)
QT6_ADD_RESOURCES(RCC_SRCS ${RCCS})

add_library(kadistudio_workfloweditor SHARED
  ${SRCS}
  ${RCC_SRCS}
)

target_include_directories(kadistudio_workfloweditor
  PRIVATE ${PROJECT_SOURCE_DIR}/plugins/application/workflowexecution
  PRIVATE ${PROJECT_SOURCE_DIR}/plugins/application/workfloweditor/thirdparty/nodes/include
  PRIVATE ${PROJECT_SOURCE_DIR}/plugins/application/workfloweditor/models/
  PRIVATE ${PROJECT_SOURCE_DIR}/plugins/application/workfloweditor/data/
)

target_compile_definitions(kadistudio_workfloweditor PUBLIC NODE_EDITOR_EXPORTS NODE_EDITOR_SHARED)

target_link_libraries(kadistudio_workfloweditor
  ${KADISTUDIO_FRAMEWORK}
  Qt6::Xml
  Qt6::Svg
  Qt6::SvgWidgets
  Qt6::PrintSupport
  QtNodes
)

string(REPLACE ${PROJECT_SOURCE_DIR} "" outputdestination ${CMAKE_CURRENT_SOURCE_DIR})

set(outputdirectory ${COMMON_LIBRARY_OUTPUT_DIRECTORY}${outputdestination})

set_target_properties(kadistudio_workfloweditor PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${outputdirectory})
