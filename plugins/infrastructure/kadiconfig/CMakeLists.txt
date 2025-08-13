set(SRCS
  kadiconfigplugin.cpp
  src/kadiconfig.cpp
)

set(RCCS kadiconfig.qrc)
QT6_ADD_RESOURCES(RCC_SRCS ${RCCS})

add_library(kadistudio_kadiconfig SHARED ${SRCS} ${RCC_SRCS})

target_link_libraries(kadistudio_kadiconfig ${KADISTUDIO_FRAMEWORK} Qt6::Network Qt6::Widgets)

string(REPLACE ${PROJECT_SOURCE_DIR} "" outputdestination ${CMAKE_CURRENT_SOURCE_DIR})

set(outputdirectory ${COMMON_LIBRARY_OUTPUT_DIRECTORY}${outputdestination})

set_target_properties(kadistudio_kadiconfig PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${outputdirectory})
