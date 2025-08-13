set(SRCS
  qpropertywidgetfactoryplugin.cpp

  src/qpropertywidgetfactory.cpp

  src/widgets/qvtiwidget_bool.cpp
  src/widgets/qvtiwidget_int.cpp
  src/widgets/qvtiwidget_long.cpp
  src/widgets/qvtiwidget_float.cpp
  src/widgets/qvtiwidget_double.cpp
  src/widgets/qvtiwidget_string.cpp
  src/widgets/qvtiwidget_textBox.cpp
  src/widgets/qvtiwidget_arcball.cpp
  src/widgets/qvtiwidget_date.cpp
  src/widgets/qvtiwidget_datetime.cpp
  src/widgets/qvtiwidget_time.cpp
  src/widgets/qvtiwidget_list.cpp
  src/widgets/qvtiwidget_combobox.cpp
  src/widgets/qpropertyfileopen.cpp
  src/widgets/qpropertypushbutton.cpp
  src/widgets/qpropertyradiobutton.cpp
  src/widgets/qtpropertypanel.cpp
  src/widgets/qvtiwidget_containerpanel.cpp
  src/widgets/qvtitooltip.cpp
  src/widgets/optionalcheckbox.h

  src/widgets/control/bordermodewidget.cpp
  src/widgets/control/boxsizewidget.cpp
  src/widgets/control/colorlabel.cpp
  src/widgets/control/colorwidget.cpp
  src/widgets/control/colorgradientlabel.cpp
  src/widgets/control/filewidget.cpp
  src/widgets/control/filterlistwidget.cpp
  src/widgets/control/listswidget.cpp
  src/widgets/control/timelinewidget.cpp
  src/widgets/control/cropimagestackwidget.cpp
  src/widgets/control/internal/cropimagewidget.cpp
  src/widgets/control/formattedtextwidget.cpp
  src/widgets/control/periodictablewidget.cpp
  src/widgets/control/positionwidget.cpp

  src/widgets/myqgroupbox.cpp
  src/widgets/qvtiwidget_vector_lineedit.cpp
  src/widgets/qvtiwidget_matrix_lineedit.cpp

  src/qwidgetinterfaceimpl.cpp
  qvtiwidget.cpp
  qwidgetinterface.h
  qpropertywidget.cpp
  userinteractioneventfilter.cpp
)

if(WEBVIEW_SUPPORT_ENABLED)
  list(APPEND SRCS src/widgets/control/webviewwidget.cpp)
endif()


add_library(kadistudio_qpropertywidgetfactory SHARED
  ${SRCS}
)

target_compile_definitions(kadistudio_qpropertywidgetfactory PRIVATE OPTIONS_EXPORTS)

target_include_directories(kadistudio_qpropertywidgetfactory
  PRIVATE ${PROJECT_SOURCE_DIR}/plugins/infrastructure/
)

target_link_libraries(kadistudio_qpropertywidgetfactory
  ${KADISTUDIO_FRAMEWORK}
  properties
  Qt6::Widgets
)

if(WEBVIEW_SUPPORT_ENABLED)
  target_link_libraries(kadistudio_qpropertywidgetfactory
    Qt6::WebEngineCore
    Qt6::WebEngineWidgets
    Qt6::Network
  )
endif()

string(REPLACE ${PROJECT_SOURCE_DIR} "" outputdestination ${CMAKE_CURRENT_SOURCE_DIR})

set(outputdirectory ${COMMON_LIBRARY_OUTPUT_DIRECTORY}${outputdestination})

set_target_properties(kadistudio_qpropertywidgetfactory PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${outputdirectory})
