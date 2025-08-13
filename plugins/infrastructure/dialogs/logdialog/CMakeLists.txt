set(SRCS
  src/logdialog.cpp
  src/logtreeitem.cpp
  src/logcontent.cpp
  src/logcontextarea.cpp
  src/logtextwidget.cpp
  src/logline.cpp
  logdialogplugin.cpp
)


set(RCCS logdialog.qrc)
QT6_ADD_RESOURCES(RCC_SRCS ${RCCS})

add_library(kadistudio_logdialog SHARED
  ${SRCS}
  ${RCC_SRCS}
)

target_link_libraries(kadistudio_logdialog ${Qt6Widgets_LIBRARIES} ${KADISTUDIO_FRAMEWORK})

string(REPLACE ${PROJECT_SOURCE_DIR} "" outputdestination ${CMAKE_CURRENT_SOURCE_DIR})

set(outputdirectory ${COMMON_LIBRARY_OUTPUT_DIRECTORY}${outputdestination})

set_target_properties(kadistudio_logdialog PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${outputdirectory})
