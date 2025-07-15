set(SRCS
  toolstarterplugin.cpp
  gui/toolstarterwidget.cpp
  gui/toolprocessdialog.cpp
  gui/toolhistorydialog.cpp
  gui/toolhistorysettings.cpp
  gui/toolbookmarks.cpp
  gui/toolbookmarksdialog.cpp
)


set(RCCS toolstarter.qrc)
QT5_ADD_RESOURCES(RCC_SRCS ${RCCS})

add_library(kadistudio_toolstarter SHARED
  ${SRCS}
  ${RCC_SRCS}
)

target_include_directories(kadistudio_toolstarter
  PRIVATE ${PROJECT_SOURCE_DIR}/plugins/infrastructure/dialogs/tooldialog
)

target_link_libraries(kadistudio_toolstarter ${KADISTUDIO_FRAMEWORK} properties ${Qt5Widgets_LIBRARIES})

string(REPLACE ${PROJECT_SOURCE_DIR} "" outputdestination ${CMAKE_CURRENT_SOURCE_DIR})

set(outputdirectory ${COMMON_LIBRARY_OUTPUT_DIRECTORY}${outputdestination})

set_target_properties(kadistudio_toolstarter PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${outputdirectory})
