set(SRCS
  testplugin.cpp
  gui/propertiestestgui.cpp
)

set(RCCS testplugin.qrc)
QT5_ADD_RESOURCES(RCC_SRCS ${RCCS})

add_library(kadistudio_testplugin SHARED ${SRCS})

target_link_libraries(kadistudio_testplugin ${KADISTUDIO_FRAMEWORK} Qt5::Widgets properties)

string(REPLACE ${PROJECT_SOURCE_DIR} "" outputdestination ${CMAKE_CURRENT_SOURCE_DIR})

set(outputdirectory ${COMMON_LIBRARY_OUTPUT_DIRECTORY}${outputdestination})

set_target_properties(kadistudio_testplugin PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${outputdirectory})
