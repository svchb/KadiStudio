/* Copyright 2025 Karlsruhe Institute of Technology
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License. */

#include <QHBoxLayout>
#include <QPushButton>
#include <QScrollArea>

#include <properties/data/properties/controls/webviewproperty.h>
#include <properties/data/properties.h>
#include <properties/data/valuetypeinterfacehint.h>

#include <plugins/infrastructure/qpropertywidgetfactory/src/widgets/qtpropertypanel.h>

#include "propertiestestgui.h"

PropertiesTestGui::PropertiesTestGui(WidgetFactoryInterface* widgetFactoryInterface, QWidget* parent) : QWidget(parent) {
  auto *layout = new QVBoxLayout();
  setLayout(layout);

  model = new PropertiesModel("test");

  // Scalar types.
  {
    model->addProperty(new BoolProperty("bool", true));
    model->addProperty(new DoubleProperty("double", 123.321));
    model->addProperty(new IntProperty("int", 456));
    model->addProperty(new LongProperty("long", 321));
    model->addProperty(new StringProperty("string", "Hello World"));

    model
      ->addProperty(new StringProperty("string_list", "A"))
      ->updateHint()->setValidator<ListValidator<std::string>>(ValidatorType::IN_LIST, std::vector<std::string> {
        "A", "B", "C"
      });

    model->addProperty(new StringProperty("date", "2024-07-10T10:13:37+02:00"))->updateHint()->setWidgetIdentifier("date");
    model->addProperty(new StringProperty("datetime", "2024-07-10T10:13:37+02:00"))->updateHint()->setWidgetIdentifier("datetime");
    model->addProperty(new StringProperty("time", "13:37"))->updateHint()->setWidgetIdentifier("time");
    model->addProperty(new StringProperty("textBox", "This is a textBox Widget"))->updateHint()->setWidgetIdentifier("textBox");
    model->addProperty(new StringProperty("vector_string", "(1,2,3,4)"))->updateHint()->setWidgetIdentifier("vector_string");
    model->addProperty(new StringProperty("matrix_string", "[(1,0,),(0,1)]"))->updateHint()->setWidgetIdentifier("matrix_string");

    model->addProperty(new ColumnProperty());
  }

  // Linked properties.
#if 1
  {
    model->addProperty(new LinkedProperty("bool_linked_property", "bool"));
    model->addProperty(new LinkedProperty("double_linked_property", "double"));
    model->addProperty(new LinkedProperty("int_linked_property", "int"));
    model->addProperty(new LinkedProperty("long_linked_property", "long"));
    model->addProperty(new LinkedProperty("string_linked_property", "string"));

    model->addProperty(new RowProperty());
  }
#endif

  // Container properties.
#if 1
  {
    model->addProperty(new ArrayProperty<float, 16>("array", std::array<float, 16> {
          1.0, 0.0, 0.0, 0.0,
          0.0, 1.0, 0.0, 0.0,
          0.0, 0.0, 1.0, 0.0,
          0.0, 0.0, 0.0, 1.0,
    }));

    model->addProperty(new ArrayProperty<float, 16>("ArcBall", std::array<float, 16> {
          1.0, 0.0, 0.0, 0.0,
          0.0, 1.0, 0.0, 0.0,
          0.0, 0.0, 1.0, 0.0,
          0.0, 0.0, 0.0, 1.0,
    }))->updateHint()->setWidgetIdentifier("arcBall");

    model->addProperty(new ColumnProperty());
  }
#endif

  // Control properties.
#if 1
  {
    model->addProperty(new BorderModeControlProperty({"Cut", "Kick", "None"}));
    model->addProperty(new CropSelectionProperty("CropSelection"));
    model->addProperty(new OpenFileProperty("FileProperty", "test.txt"));

    auto colorcontrol = model->addProperty(new ColorControlProperty("ColorModel"));
    colorcontrol->setValue("colormax",  1.0f);
    colorcontrol->setValue("colormin", -1.0f);

    model->addProperty(new PeriodicTableSelectionProperty("PeriodicTableSelection"));

    auto formatted_text = new FormattedTextProperty("FormattedText");
    formatted_text->fromString("[markdown] # Hello World");

    // TODO: Currently not used because it breaks the layout.
    // It needs so much space that a horizontal scrollbar appears, which does not look good.
    // model->addProperty(new WebViewProperty("WebViewProperty"));

    model->addProperty(new BoundingBoxControlProperty("bounding_box", -1, 1, -2, 2, -3, 3));

    model->addProperty(new CuttingPlaneControlProperty(0, 10, 100, 200));

    model->addProperty(new TimeLineControlProperty(/*maxtimesteps*/10, /*delay*/1.0, /*selected*/"default", /*timelines*/{"default", "extended", "special"}));

    model->addProperty(new ActiveSetControlProperty("dummyset", std::vector<std::string>{"ABC", "BCD"}, std::vector<std::string>{"CDE", "DEF"}));

    model->addProperty(new StringListElementProperty("File", "cde", {"abc", "bcd", "cde", "eff"}))->updateHint()->setWidgetIdentifier("fileExtension");
  }
#endif

  // Sub models
#if 1
  {
    PropertiesModel *sub_model = new PropertiesModel("sub");

    sub_model->addProperty(new BoolProperty("bool", true));
    sub_model->addProperty(new DoubleProperty("double", 234.567));
    sub_model->addProperty(new IntProperty("int", 789));
    sub_model->addProperty(new LongProperty("long", 654));
    sub_model->addProperty(new StringProperty("string", "Hello Ambassador"));

    sub_model->addProperty(new LinkedProperty("bool_linked_property", "../bool"));
    sub_model->addProperty(new LinkedProperty("double_linked_property", "../double"));
    sub_model->addProperty(new LinkedProperty("int_linked_property", "../int"));
    sub_model->addProperty(new LinkedProperty("long_linked_property", "../long"));
    sub_model->addProperty(new LinkedProperty("string_linked_property", "../string"));

    model->addProperty(sub_model);
  }
#endif

#if 0
  auto *long_vector_property = new VectorProperty<long>("longVectorProperty", {0, 1, 2, 3, 4, 5});
  ValueTypeInterfaceHint *long_vector_property_hint = long_vector_property->updateHint();
  long_vector_property_hint->setEntry("label", "Please enter a numeric vector:");
  model->addProperty(long_vector_property);
#endif

#if 0
  auto dynamic = model->addProperty(new DynamicProperty("dynamic", model));
  auto dynamicinterface = model->getProperty<DynamicProperty>("dynamic");
  dynamicinterface->addValue(10);
  dynamicinterface->addValue(5.15f);
  dynamicinterface->addValue(false);
  assert(*dynamicinterface->at<int>(0) == 10);
  assert(*dynamicinterface->at<float>(1) == 5.15f);
  assert(*dynamicinterface->at<bool>(2) == false);
  AbstractValueTypeInterface *next_element = model->getPropertyVTI("dynamic").begin();
  assert(next_element->getValueTypeInfo() == typeid(int));
  next_element = next_element->next();
  assert(next_element->getValueTypeInfo() == typeid(float));
  next_element = next_element->next();
  assert(next_element->getValueTypeInfo() == typeid(bool));

  dynamicinterface->set(0, 31);
  assert(*dynamicinterface->at<int>(0) == 31);

  assert(dynamic->toString() == "31,5.15,0");
  dynamic->fromString("15,123,1");
  assert(*dynamicinterface->at<int>(0) == 15);
  assert(*dynamicinterface->at<float>(1) == 123.f);
  assert(*dynamicinterface->at<bool>(2) == true);
  assert(dynamic->toString() == "15,123,1");

  dynamicinterface->addDelimiter(',');
  dynamicinterface->addDelimiter('x');
  assert(dynamic->toString() == "15,123x1");
  dynamic->fromString("9,5.15x0");
  assert(*dynamicinterface->at<int>(0) == 9);
  assert(*dynamicinterface->at<float>(1) == 5.15f);
  assert(*dynamicinterface->at<bool>(2) == false);
#endif

#if 0
  auto *matrixproperty = model->addProperty(new MatrixProperty<long>("matrix", 4, 4, 0));
  auto *matrixinterface = model->getProperty<MatrixProperty<long>>("matrix");
  matrixinterface->set(1, 2, 17);
  assert(matrixproperty->toString() == "[(0, 0, 0, 0), (0, 0, 17, 0), (0, 0, 0, 0), (0, 0, 0, 0)]");
  matrixproperty->fromString("[(0, 0, 0, 0), (0, 0, 17, 0), (0, 0, 111, 0), (0, 0, 18, 0)]");
  assert(matrixinterface->at(3, 2) == 18);
  matrixproperty->fromString("[(0,    3, 0, 0), (0, 1, 17, 0),      (0,0, 111, 0), (0, 0,-18, 0)]");
  assert(matrixinterface->at(0, 1) == 3);
  assert(matrixinterface->at(1, 1) == 1);
  assert(matrixinterface->at(2, 2) == 111);
  assert(matrixinterface->at(3, 2) == -18);
  bool invalidcatched = false;
  try {
    matrixproperty->fromString("[(0, 3, 3, 3, 3), (0)]"); // column length must be the same for all rows
  } catch(const std::invalid_argument&) {
    invalidcatched = true;
  }
  assert(invalidcatched);
#endif

#if 0
  auto *emptymatrix = model->addProperty(new MatrixProperty<float>("emptyInitializedMatrix", 0, 0, 0.0));
  emptymatrix->fromString("[(0.0)]");
#endif

#if 0
  auto complex = new ComplexProperty<int, float, std::string>("complex", model, 7, 3.818, "test");
  model->addProperty(complex);
  auto complexinterface = model->getProperty<ComplexInterface<int, float, std::string>>("complex");
  assert(complexinterface->get<0>() == 7);
  assert(complexinterface->get<1>() == 3.818f);
  assert(complexinterface->get<2>() == "test");
  next_element = model->getPropertyVTI("complex").begin();
  assert(next_element->getValueTypeInfo() == typeid(int));
  next_element = next_element->next();
  assert(next_element->getValueTypeInfo() == typeid(float));
  next_element = next_element->next();
  assert(next_element->getValueTypeInfo() == typeid(std::string));

  assert(complex->toString() == "7,3.818,test");
  complex->fromString("8,9.1,foobar");
  assert(complexinterface->get<0>() == 8);
  assert(complexinterface->get<1>() == 9.1f);
  assert(complexinterface->get<2>() == "foobar");

  complexinterface->addDelimiter('_');
  assert(complex->toString() == "8_9.1,foobar");
#endif

  model->dump();

  PropertyPanel *panel = widgetFactoryInterface->createScollableGui(model, this);
}
