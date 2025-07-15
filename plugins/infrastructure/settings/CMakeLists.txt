set(SRCS
  gui/settingsgui.cpp
  settingsplugin.cpp
  settingsdelegate.cpp
)

set(RCCS settings.qrc)
QT5_ADD_RESOURCES(RCC_SRCS ${RCCS})

add_library(kadistudio_settingsplugin SHARED
  ${SRCS}
  ${RCC_SRCS}
  ${UIS_H}
)

target_link_libraries(kadistudio_settingsplugin ${KADISTUDIO_FRAMEWORK} Qt5::Widgets properties)

string(REPLACE ${PROJECT_SOURCE_DIR} "" outputdestination ${CMAKE_CURRENT_SOURCE_DIR})

set(outputdirectory ${COMMON_LIBRARY_OUTPUT_DIRECTORY}${outputdestination})

set_target_properties(kadistudio_settingsplugin PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${outputdirectory})
