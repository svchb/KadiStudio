set(SRCS
  editdialog.cpp
)

set(RCCS resources_icons.qrc)
QT6_ADD_RESOURCES(RCC_SRCS ${RCCS})

add_library(kadistudio_widgetdialog SHARED ${SRCS} ${RCC_SRCS})

target_include_directories(kadistudio_widgetdialog
  PRIVATE ${PROJECT_SOURCE_DIR}/
)

target_link_libraries(kadistudio_widgetdialog
  ${KADISTUDIO_FRAMEWORK}
  Qt6::Widgets
)

string(REPLACE ${PROJECT_SOURCE_DIR} "" outputdestination ${CMAKE_CURRENT_SOURCE_DIR})

set(outputdirectory ${COMMON_LIBRARY_OUTPUT_DIRECTORY}${outputdestination})

set_target_properties(kadistudio_widgetdialog PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${outputdirectory})
