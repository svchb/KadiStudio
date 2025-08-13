find_package(Qt6 COMPONENTS Widgets Gui REQUIRED)
set(QT_USE_QTGUI TRUE)
set(QT_USE_QTOPENGL TRUE)
set(QT_USE_OPENGL TRUE)
set(QT_USE_QTXML TRUE)
set(QT_USE_QTNETWORK TRUE)
set(QT_USE_QTTEST TRUE)
set(QT_USE_QTOPENGL TRUE)

include_directories(${PROJECT_SOURCE_DIR}/
                    ${PROJECT_SOURCE_DIR}/..
                    ${PROJECT_SOURCE_DIR}/../lib
                    ${QT_INCLUDE_DIR}
                    ${CMAKE_CURRENT_BINARY_DIR}
                    ${QT_QtOpenGL_INCLUDE_DIR})

add_definitions(${QT_DEFINITIONS})
add_definitions(-DQT_DLL)
add_definitions(-DQT_PLUGIN)
add_definitions(-DQT_NO_DEBUG)
add_definitions(-DQT_SHARED)
add_definitions(-DQT_THREAD_SUPPORT)

macro(ADD_KADISTUDIO_TEST target docstr filenames libraries)
  set(TEST_FILES_NETWORK
      "test_network/test_network_tooldataconverter.cpp"
      "test_network/test_network_filechain.cpp"
      "test_network/test_network_filechaindevided.cpp"
      "test_network/test_network_socketchain.cpp"
      "test_network/test_network_port.cpp")

  set(TEST_FILES_TOOLDIALOG
      "test_tooldialog/test_tooldialog_settings.cpp"
      "test_tooldialog/test_tooldialog_choosetool.cpp")

  if(${filenames} MATCHES test_network)
    add_executable(${target} ${filenames} ${TEST_FILES_NETWORK})
  elseif(${filenames} MATCHES test_tooldialog)
    add_executable(${target} ${filenames} ${TEST_FILES_TOOLDIALOG})
  else()
    add_executable(${target} ${filenames})
  endif()

  target_link_libraries(${target} kadistudio_framework ${libraries} ${QT_LIBRARIES})
  add_test("${docstr}" ${COMMON_RUNTIME_OUTPUT_DIRECTORY}/${target})
endmacro(ADD_KADISTUDIO_TEST)

macro(ADD_KADISTUDIOPLUGIN_TEST basename)
  ADD_KADISTUDIO_TEST(test_${basename} ${basename} test_${basename}.cpp kadistudio_${basename})
endmacro(ADD_KADISTUDIOPLUGIN_TEST)

ADD_KADISTUDIOPLUGIN_TEST(network)
ADD_KADISTUDIOPLUGIN_TEST(tooldialog)
