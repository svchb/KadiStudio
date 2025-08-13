include_directories(
  ${PROJECT_SOURCE_DIR}/plugins/infrastructure/qpropertywidgetfactory
)

set(SRCS
  propertyformwidgetplugin.cpp
  src/propertyformwidget.cpp
)

add_library(kadistudio_propertyformwidget SHARED
  ${SRCS}
)

target_link_libraries(kadistudio_propertyformwidget ${KADISTUDIO_FRAMEWORK} properties Qt6::Xml Qt6::Widgets)

string(REPLACE ${PROJECT_SOURCE_DIR} "" outputdestination ${CMAKE_CURRENT_SOURCE_DIR})

set(outputdirectory ${COMMON_LIBRARY_OUTPUT_DIRECTORY}${outputdestination})

set_target_properties(kadistudio_propertyformwidget PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${outputdirectory})
