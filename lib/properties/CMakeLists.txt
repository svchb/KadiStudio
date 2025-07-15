set(properties_SOURCE
  data/ambassador.cpp
  data/propertiesmodel.cpp
  data/property.cpp
  data/propertyfactory.cpp
  data/valuetypeinterfacehint.cpp

  data/properties/linkedproperty.cpp

  data/properties/controls/boundingboxcontrolproperty.cpp
  data/properties/controls/colorcontrolproperty.cpp
  data/properties/controls/colormapproperty.cpp
  data/properties/controls/columnproperty.cpp
  data/properties/controls/cropselectionproperty.cpp
  data/properties/controls/formattedtextproperty.cpp
  data/properties/controls/periodictableselectionproperty.cpp
  data/properties/controls/webviewproperty.cpp

  data/valuetypeinterface/abstractvaluetypeinterface.cpp

  data/colormap.c

  ui/factory/propertypanel.cpp
  ui/factory/propertywidgetfactory.cpp
  ui/factory/vtiwidget.cpp

)

include_directories(
  ${PROJECT_SOURCE_DIR}/lib/cpputils/include
)


add_library(properties SHARED ${properties_SOURCE})

target_link_libraries(properties PUBLIC cpputils)
