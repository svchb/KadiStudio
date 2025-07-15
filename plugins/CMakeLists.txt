find_package(Qt5 COMPONENTS Widgets Gui OpenGL Xml Network Test Svg REQUIRED)
add_definitions(${Qt5Widgets_DEFINITIONS})

# Enable webview support if qt version >= 5.11.3
set(WEBVIEW_REQUIRED_QT_VERSION 5.11.3)
if (Qt5_FOUND)
  if (Qt5_VERSION VERSION_GREATER_EQUAL WEBVIEW_REQUIRED_QT_VERSION)
    set(WEBVIEW_SUPPORT_ENABLED TRUE)
    add_compile_definitions(WEBVIEW_SUPPORT_ENABLED)
    find_package(Qt5 REQUIRED COMPONENTS WebEngineWidgets WebEngine)
    # We need add -DQT_WIDGETS_LIB when using QtWidgets in Qt 5.
    add_definitions(
      ${Qt5WebEngineWidgets_DEFINITIONS}
    )
    message(STATUS "Enabling WebView support (Qt version: ${Qt5_VERSION})")
  endif()
endif()

add_definitions(-DQT_THREAD_SUPPORT)

include_directories(
  ${PROJECT_SOURCE_DIR}/src
  ${PROJECT_SOURCE_DIR}/
  # The Qt5Widgets_INCLUDES also includes the include directories for
  # dependencies QtCore and QtGui
  ${Qt5Widgets_INCLUDES}
)
if (WEBVIEW_SUPPORT_ENABLED)
  include_directories(${Qt5WebEngine_INCLUDES} ${Qt5WebEngineWidgets_INCLUDES})
endif()

set(CMAKE_INCLUDE_CURRENT_DIR ON)

option(KADISTUDIO_COMPILE_PLUGINS ON)

add_subdirectory(infrastructure)
add_subdirectory(application)
