find_package(Qt6 COMPONENTS Widgets Gui REQUIRED)

# We need add -DQT_WIDGETS_LIB when using QtWidgets in Qt 5.
add_definitions(${Qt6Widgets_DEFINITIONS})


set(CMAKE_INCLUDE_CURRENT_DIR ON)

# framework library
set(RCCS framework.qrc)
QT6_ADD_RESOURCES(RCC_SRCS ${RCCS})

add_library(kadistudio_framework SHARED
  ${RCC_SRCS}
)

target_link_libraries(kadistudio_framework ${Qt6Widgets_LIBRARIES})

target_include_directories(kadistudio_framework PUBLIC
  # The Qt6Widgets_INCLUDES also includes the include directories for
  # dependencies QtCore and QtGui
  SYSTEM ${Qt6Widgets_INCLUDE_DIRS}
  ${PROJECT_SOURCE_DIR}/lib/
  ${PROJECT_SOURCE_DIR}/lib/cpputils/include/
  ${PROJECT_SOURCE_DIR}/src/
)

target_compile_definitions(kadistudio_framework PRIVATE OPTIONS_EXPORTS)

set_target_properties(kadistudio_framework PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${COMMON_LIBRARY_OUTPUT_DIRECTORY})

add_subdirectory(mainwindow)
add_subdirectory(dock)
add_subdirectory(workspace)
add_subdirectory(tab)
add_subdirectory(menu)
add_subdirectory(statusbar)
add_subdirectory(commandlineparser)
add_subdirectory(pluginframework)
add_subdirectory(enhanced)
