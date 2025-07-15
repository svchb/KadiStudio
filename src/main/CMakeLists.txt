find_package(Qt5 COMPONENTS Widgets Gui REQUIRED)

include_directories(
  ${PROJECT_SOURCE_DIR}/framework/
  SYSTEM ${Qt5Widgets_INCLUDE_DIRS}
)

set(SRCS
  application.cpp
  kadistudio.cpp
)

add_executable(kadistudio
  ${SRCS}
)

target_link_libraries(kadistudio
  ${Qt5Widgets_LIBRARIES}
  ${KADISTUDIO_FRAMEWORK}
  ${CMAKE_DL_LIBS}
)

if(CMAKE_BUILD_TYPE MATCHES Debug)
  set_target_properties(kadistudio PROPERTIES LINK_FLAGS "-Wl,-rpath ${COMMON_LIBRARY_OUTPUT_DIRECTORY}")
endif(CMAKE_BUILD_TYPE MATCHES Debug)

set_target_properties(kadistudio PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${COMMON_RUNTIME_OUTPUT_DIRECTORY}")
