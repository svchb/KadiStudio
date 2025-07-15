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

#include <QMap>

#include <cpputils/dllapi.hpp>

#include <framework/pluginframework/pluginmanager.h>

#include "properties/data/valuetypeinterface/abstractvaluetypeinterface.h"
#include "properties/data/valuetypeinterface/arrayvaluetype.h"
#include "properties/data/valuetypeinterface/vectorvaluetype.h"
#include "properties/data/valuetypeinterface/matrixvaluetype.h"

#include "widgets/control/listswidget.h"
#include "widgets/control/boxsizewidget.h"
#include "widgets/control/bordermodewidget.h"
#include "widgets/control/colorwidget.h"
#include "widgets/control/filewidget.h"
#include "widgets/control/formattedtextwidget.h"
#include "widgets/control/periodictablewidget.h"
#include "widgets/control/cropimagestackwidget.h"
#include "widgets/control/positionwidget.h"
#include "widgets/control/timelinewidget.h"
#include "widgets/myqgroupbox.h"
#ifdef WEBVIEW_SUPPORT_ENABLED
#include "widgets/control/webviewwidget.h"
#endif

#include "widgets/qvtiwidget_bool.h"
#include "widgets/qvtiwidget_int.h"
#include "widgets/qvtiwidget_long.h"
#include "widgets/qvtiwidget_float.h"
#include "widgets/qvtiwidget_double.h"
#include "widgets/qvtiwidget_string.h"
#include "widgets/qvtiwidget_arcball.h"
#include "widgets/qvtiwidget_date.h"
#include "widgets/qvtiwidget_datetime.h"
#include "widgets/qvtiwidget_time.h"
#include "widgets/qvtiwidget_textBox.h"
#include "widgets/qvtiwidget_list.h"
#include "widgets/qvtiwidget_combobox.h"
#include "widgets/qpropertyfileopen.h"
#include "widgets/qpropertypushbutton.h"
#include "widgets/qtpropertypanel.h"
#include "widgets/qvtiwidget_containerpanel.h"

#include "widgets/qvtiwidget_vector_lineedit.h"
#include "widgets/qvtiwidget_matrix_lineedit.h"

#include <plugins/infrastructure/widgetdialog/editdialoginterface.h>

#include "qpropertywidgetfactory.h"

template<typename T>
static PropertyWidgetFactory::CreateWidgetFunc vtiCreatorFunc() {
  return [](AbstractValueTypeInterface* vti) {
    return new T(vti);
  };
}

template<typename T>
static PropertyWidgetFactory::CreateWidgetFunc propertyCreatorFunc() {
  return [](AbstractValueTypeInterface* vti) {
    return new T(dynamic_cast<Property*>(vti));
  };
}

template QPropertyWidgetFactory* Singleton<QPropertyWidgetFactory>::getInstance();

QPropertyWidgetFactory::QPropertyWidgetFactory() {
  // - missing widget for AlternativeColorControlProperty, original comment:
  //   TODO implement a widget class

  this->pluginmanager = LibFramework::PluginManager::getInstance();

  registerWidget<ValueTypeInterface<std::string>>("", [](AbstractValueTypeInterface* vti) -> VTIWidget* {
    const ValueValidator *validator = vti->getHint()->getValidator<ValueValidator>();

    if (validator && validator->getValidatorType() == ValidatorType::IN_LIST) {
      return vtiCreatorFunc<QVTIWidget_combobox>()(vti);
    }

    return vtiCreatorFunc<QVTIWidget_string>()(vti);
  });

  registerWidget<ValueTypeInterface<std::string>>("file", [&](AbstractValueTypeInterface* vti) -> VTIWidget* {
    return new QPropertyFileOpen(pluginmanager, dynamic_cast<Property*>(vti));
  });

  registerWidget<ValueTypeInterface<std::string>>("date", vtiCreatorFunc<QVTIWidget_date>());
  registerWidget<ValueTypeInterface<std::string>>("datetime", vtiCreatorFunc<QVTIWidget_datetime>());
  registerWidget<ValueTypeInterface<std::string>>("time", vtiCreatorFunc<QVTIWidget_time>());
  registerWidget<ValueTypeInterface<std::string>>("textBox", vtiCreatorFunc<QVTIWidget_textBox>());
  registerWidget<VectorValueType<std::string>>("vector_string", vtiCreatorFunc<QVTIWidget_Vector_lineedit>());
  registerWidget<MatrixValueType<std::string>>("matrix_string", vtiCreatorFunc<QVTIWidget_Matrix_lineedit>());

  registerWidget<ValueTypeInterface<bool>>("", vtiCreatorFunc<QVTIWidget_bool>());
  registerWidget<ValueTypeInterface<double>>("", vtiCreatorFunc<QVTIWidget_double>());
  registerWidget<ValueTypeInterface<float>>("", vtiCreatorFunc<QVTIWidget_float>());
  registerWidget<ValueTypeInterface<int>>("",  vtiCreatorFunc<QVTIWidget_int>());
  registerWidget<ValueTypeInterface<long>>("", vtiCreatorFunc<QVTIWidget_long>());

  registerWidget<ArrayValueType<float, 16>>("arcBall", vtiCreatorFunc<QVTIWidget_arcball>());
  registerWidget<ValueTypeInterface<std::pair<std::vector<std::string>, std::vector<std::string>>>>("ActiveSetControl", propertyCreatorFunc<ListsWidget>());

  registerWidget<void>("pushButton", propertyCreatorFunc<QPropertyPushButton>());

  registerAmbassadorWidget("CropSelection", "cropImageStackWidget", propertyCreatorFunc<CropImageStackWidget>());
  registerAmbassadorWidget("FormattedText", "formattedTextWidget", propertyCreatorFunc<FormattedTextWidget>());
  registerAmbassadorWidget("PeriodicTableSelection", "periodicTableWidget", propertyCreatorFunc<PeriodicTableWidget>());
  registerAmbassadorWidget("ColorControl", "", propertyCreatorFunc<ColorWidget>());

  // GLViewer
  registerAmbassadorWidget("BorderModeControl", "", propertyCreatorFunc<BorderModeWidget>());
  registerAmbassadorWidget("BoundingBox", "", propertyCreatorFunc<BoxSizeWidget>());
  registerAmbassadorWidget("StringList", "fileExtension", propertyCreatorFunc<FileWidget>());
  registerAmbassadorWidget("CuttingPlaneControl", "", propertyCreatorFunc<PositionWidget>());
  registerAmbassadorWidget("TimeLine", "", propertyCreatorFunc<TimelineWidget>());

#ifdef WEBVIEW_SUPPORT_ENABLED
  registerAmbassadorWidget("WebView", "", propertyCreatorFunc<WebViewWidget>());
#endif

  // loading addon dialogs for specific ValueTypes
  std::string keytypename = std::string("/plugins/infrastructure/widgetdialog/typedialog/");

  if (!pluginmanager->isLoaded(keytypename)) {
    pluginmanager->load(keytypename);
  }

  auto editdialog_interfaces = pluginmanager->getInterfaces(keytypename);
  for (const auto &editdialog_interface : editdialog_interfaces) {
    auto editdialogs = editdialog_interface.second->getInterfaces<EditDialogInterface*>();
    for (auto editdialog : editdialogs) {
      for (const auto &addondialogtype : editdialog->getUsableTypes()) {
        addondialogs[addondialogtype].append(editdialog);
      }
    }
  }
}

PropertyPanel* QPropertyWidgetFactory::createPanel(Ambassador* ambassador) {
  if (!ambassador->getHint()->hasEntry("label")) {
    ambassador->updateHint()->setEntry("label", ambassador->getName());
    ambassador->updateHint()->setEntry("label.pos", "top");
  }

  return new QtPropertyPanel(ambassador, new MyQGroupBox("", new QGridLayout()));
}

PropertyPanel* QPropertyWidgetFactory::createContainerPanel(AbstractValueTypeInterface* vti, bool issubcontainer) {
  return new QVTIWidget_ContainerPanel(vti, issubcontainer, new MyQGroupBox("", new QGridLayout()));
}

void QPropertyWidgetFactory::finalizeWidget(VTIWidget* vtiwidget) {
  auto valuetypeinterface = vtiwidget->getValueTypeInterface();
  auto qwidgetinterface = dynamic_cast<QWidgetInterface*>(vtiwidget);

  auto hint = valuetypeinterface->getHint();

  if (auto property = dynamic_cast<Property*>(valuetypeinterface)) {
    qwidgetinterface->getWidget()->setObjectName(property->getName().c_str());
    qwidgetinterface->getWidget()->setAccessibleName(property->getName().c_str());
  }

  addDialogToWidget(qwidgetinterface);

  if (hint->getEntry("editable") == "false") {
    qwidgetinterface->getWidget()->setEnabled(false);
  }
}

PropertyPanel* QPropertyWidgetFactory::createGui(Ambassador *ambassador, QWidget* parent, const std::set<std::string>& excluded_properties) {
  auto panel = static_cast<QtPropertyPanel*>(PropertyWidgetFactory::createGui(ambassador, excluded_properties));

  if (parent) {
    panel->setParent(parent);
  }

  return panel;
}

PropertyPanel* QPropertyWidgetFactory::createScollableGui(Ambassador* ambassador, QWidget* parent, const std::set<std::string>& excluded_properties) {
  PropertyPanel *panel = createGui(ambassador, parent, excluded_properties);

  auto scroll_area = new QScrollArea(parent);
  scroll_area->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
  scroll_area->setWidgetResizable(true);
  scroll_area->setWidget(static_cast<QtPropertyPanel*>(panel)->getWidget());

  parent->layout()->addWidget(scroll_area);

  return panel;
}

/** @brief adds suitable edit dialogs to a widget
  *
  * @param widget VTI widget where dialogs are added
  */
void QPropertyWidgetFactory::addDialogToWidget(QWidgetInterface* widget) {

  QMap<const std::type_info*, QList<EditDialogInterface*>>::const_iterator dialogs = addondialogs.constBegin();
  while (dialogs != addondialogs.constEnd()) {
    const std::type_info *info = dialogs.key();
    if (widget->getValueTypeInterface()->isDerivedFromBaseClass(*info)) {
      QList<EditDialogInterface*> addondialoglist;
      for (auto editdialog : dialogs.value()) {
        widget->addDialog(editdialog);
      }
    }
    ++dialogs;
  }

#if 0
  std::string dirname = std::string("/plugins/infrastructure/widgetdialog/namedialog/") + widget->getName();

  if (!pluginmanager->isLoaded(dirname)) {
    pluginmanager->load(dirname);
  }

  if (pluginmanager->isLoaded(dirname)) {
    DialogContainer *dialogcontainer = pluginmanager->getInterface<DialogContainer*>(dirname);

    QList<EditDialog*> *list = dialogcontainer->getDialogList(this, widget);

    if (list) {
      for (EditDialog *dialog : *list) {
        widget->addDialog(dialog, dialog->getIcon());
      }
    }
  }
#endif
}
