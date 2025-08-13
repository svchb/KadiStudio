set(SRCS
  ../../editdialog.cpp
  vectordialogplugin.cpp
  editdialog_vector.cpp
  editdialog_vectorfill.cpp
)

set(RCCS ../../resources_icons.qrc)
QT6_ADD_RESOURCES(RCC_SRCS ${RCCS})

add_library(kadistudio_vectordialog SHARED ${SRCS} ${RCC_SRCS})

target_include_directories(kadistudio_vectordialog
  PRIVATE ${PROJECT_SOURCE_DIR}/
)

target_link_libraries(kadistudio_vectordialog ${KADISTUDIO_FRAMEWORK} ${Qt6Widgets_LIBRARIES})

string(REPLACE ${PROJECT_SOURCE_DIR} "" outputdestination ${CMAKE_CURRENT_SOURCE_DIR})

set(outputdirectory ${COMMON_LIBRARY_OUTPUT_DIRECTORY}${outputdestination})

set_target_properties(kadistudio_vectordialog PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${outputdirectory})
