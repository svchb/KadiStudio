set(SRCS
  src/menupluginchooser.cpp
  src/shortcutmenu.cpp
  menupluginchooserplugin.cpp
)

add_library(kadistudio_menupluginchooser SHARED
  ${SRCS}
)

target_link_libraries(kadistudio_menupluginchooser ${KADISTUDIO_FRAMEWORK} ${Qt6Widgets_LIBRARIES})

string(REPLACE ${PROJECT_SOURCE_DIR} "" outputdestination ${CMAKE_CURRENT_SOURCE_DIR})

set(outputdirectory ${COMMON_LIBRARY_OUTPUT_DIRECTORY}${outputdestination})

set_target_properties(kadistudio_menupluginchooser PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${outputdirectory})
