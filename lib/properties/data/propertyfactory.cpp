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

#include "valuetypeinterfacehint.h"
#include "validator.h"
#include "property.h"
#include "properties.h"
#include "propertyfactory.h"

#include "wrapper.h"
extern "C" {
  #include "keytype.h"
}

Property* PropertyFactory::createProperty(const std::string& name, Keytype* keytype, const std::string& default_value, const std::vector<std::string>& options) {

  Property *property{};

/*
    Keytype keytype;
  if (type) {
    KeytypeIter iter;
    KeytypeIter_getFirst(&iter, type);*/
  if (keytype->type & (KEYTYPE_COMPOSITEMASK)) {
    if (keytype->type & KEYTYPE_VECTOR) {
      char *dim = Keytype_getDimensionFromVariable(keytype, 0);
      long rowcount;
      try {
        rowcount = std::stoi(dim);
      } catch (...) {
        rowcount = 0; // default to 0
      }
      Free(dim);
      // mydebug("%ld -> %ld", keytype->type, rowcount);
      if (rowcount == 0 || keytype->type & KEYTYPE_UNKNOWN || keytype->type & KEYTYPE_VALIDATOR || keytype->type & KEYTYPE_STRING) {
        property = new VectorProperty<std::string>(name, rowcount, std::string(""));
        property->updateHint()->setWidgetIdentifier("vector_string");
      } else if (keytype->type & KEYTYPE_INT) {
        property = new VectorProperty<int>(name, rowcount, 0l);
      } else if (keytype->type & KEYTYPE_FLOAT) {
        property = new VectorProperty<double>(name, rowcount, 0.0f);
      } else if (keytype->type & KEYTYPE_BOOL) {
        property = new VectorProperty<bool>(name, rowcount, false);
      // } else if (keytype->type & KEYTYPE_OPTION) {
        // property = new StringProperty(name, "");
        // if (!options.empty()) {
        //   std::string value = default_value;
        //   if (default_value.empty()) {
        //     default_value = options[0].substr(0, options[0].find("@"));
        //     std::cout << "found " << default_value << std::endl;
        //   }
        //   ValueTypeInterfaceHint *hint = property->updateHint();
        //   hint->setValidator<ListValidator<std::string>>(ValidatorType::IN_LIST, options);
        // } else {
        //   std::cerr << "No Option-Parameters found for key " << name << "!" << std::endl;
        // }
      // } else if (keytype->type & KEYTYPE_FILE) {
      //   property = new OpenFileProperty(name, "");
      //   property->updateHint()->setEntry("allowed_extensions", "All files or folders (*)");
      } else {
        std::cerr << "Type '" << keytype->type << "' (" << Keytype_getPrettyPrint(keytype) << ") for key '" << name << "' is not known in the property factory. Using string as a fallback" << std::endl;
        property = new StringProperty(name, "");
      }

      // mydebug("name %s default_value %s", name.c_str(), default_value.c_str());

    } else if (keytype->type & KEYTYPE_MATRIX) {
      long rowcount;
      char *dim0 = Keytype_getDimensionFromVariable(keytype, 0);
      try {
        rowcount = std::stoi(dim0);
      } catch (...) {
        rowcount = 0; // default to 0
      }
      Free(dim0);
      char *dim1 = Keytype_getDimensionFromVariable(keytype, 1);
      long columncount;
      try {
        columncount = std::stoi(dim1);
      } catch (...) {
        columncount = 0; // default to 0
      }
      Free(dim1);

      if (rowcount == 0 || columncount == 0 || keytype->type & KEYTYPE_UNKNOWN || keytype->type & KEYTYPE_VALIDATOR || keytype->type & KEYTYPE_STRING) {
        property = new MatrixProperty<std::string>(name, rowcount, columncount, std::string(""));
        property->updateHint()->setWidgetIdentifier("matrix_string");
      } else if (keytype->type & KEYTYPE_INT) {
        property = new MatrixProperty<int>(name, rowcount, columncount, 0l);
      } else if (keytype->type & KEYTYPE_FLOAT) {
        property = new MatrixProperty<double>(name, rowcount, columncount, 0.0f);
      } else if (keytype->type & KEYTYPE_BOOL) {
        property = new MatrixProperty<bool>(name, rowcount, columncount, false);
      // } else if (keytype->type & KEYTYPE_OPTION) {
        // property = new StringProperty(name, "");
        // if (!options.empty()) {
        //   std::string value = default_value;
        //   if (default_value.empty()) {
        //     default_value = options[0].substr(0, options[0].find("@"));
        //     std::cout << "found " << default_value << std::endl;
        //   }
        //   ValueTypeInterfaceHint *hint = property->updateHint();
        //   hint->setValidator<ListValidator<std::string>>(ValidatorType::IN_LIST, options);
        // } else {
        //   std::cerr << "No Option-Parameters found for key " << name << "!" << std::endl;
        // }
      // } else if (keytype->type & KEYTYPE_FILE) {
      //   property = new OpenFileProperty(name, "");
      //   property->updateHint()->setEntry("allowed_extensions", "All files or folders (*)");
      } else {
        std::cerr << "Type '" << keytype->type << "' (" << Keytype_getPrettyPrint(keytype) << ") for key '" << name << "' is not known in the property factory. Using string as a fallback" << std::endl;
        property = new StringProperty(name, "");
      }
    } else {
      property = new PropertiesModel(name);
      // TODO
    }
    // return property;

  } else if (keytype->type & KEYTYPE_UNKNOWN || keytype->type & KEYTYPE_VALIDATOR || keytype->type & KEYTYPE_STRING) {
    property = new StringProperty(name, "");
  } else if (keytype->type & KEYTYPE_INT) {
    property = new LongProperty(name, 0L);
  } else if (keytype->type & KEYTYPE_FLOAT) {
    property = new DoubleProperty(name, 0.f);
  } else if (keytype->type & KEYTYPE_BOOL) {
    property = new BoolProperty(name, false);
  } else if (keytype->type & KEYTYPE_NAMESPACEMODULE) {
    // curwidget = new QPropertyWidgetContainer(parent, nullptr, name.toStdString().c_str(), true); // TODO das gibt es nicht wirklich
    // type is option
    throw std::invalid_argument("Namespace module currently not supported");
  } else if (keytype->type & KEYTYPE_OPTION) {
    property = new StringProperty(name, "");
    if (!options.empty()) {
      ValueTypeInterfaceHint *hint = property->updateHint();
      hint->setValidator<ListValidator<std::string>>(ValidatorType::IN_LIST, options);
    } else {
      std::cerr << "No Option-Parameters found for key " << name << "!" << std::endl;
    }
  } else if (keytype->type & KEYTYPE_FILE) {
    property = new OpenFileProperty(name, "");
    property->updateHint()->setEntry("allowed_extensions", "All files or folders (*)");
  } else {
    std::cerr << "Type '" << keytype->type << "' (" << Keytype_getPrettyPrint(keytype) << ") for key '" << name << "' is not known in the property factory. Using string as a fallback" << std::endl;
    property = new StringProperty(name, "");
  }

  if (!default_value.empty()) {
    property->fromString(default_value);
    ValueTypeInterfaceHint *hint = property->updateHint();
    hint->setEntry("default", default_value);
  }

  return property;
}

Property* PropertyFactory::createProperty(const std::string& name, const std::type_info& info, const std::string& default_value, const std::vector<std::string>& options) {
  Property *property{};

  if (info == typeid(int)) {
    property = new IntProperty(name, 0);
  } else if (info == typeid(long)) {
    property = new LongProperty(name, 0L);
  } else if (info == typeid(float)) {
    property = new FloatProperty(name, 0.0f);
  } else if (info == typeid(REAL)) {
    property = new DoubleProperty(name, 0.0);
  } else if (info == typeid(bool)) {
    property = new BoolProperty(name, false);
  } else {
    if (info == typeid(std::string)) {
      std::cerr << "Type '" << info.name() << "' (" << demangle(info.name()) << ") for key '" << name << "' is not known in the property factory. Using string as a fallback" << std::endl;
    }
    property = new StringProperty(name, "");
  }

  return property;
}

Property *PropertyFactory::createProperty(const std::string& name, const std::string& type_name, bool isoptional,
                                          const std::string& default_value) {
  long type_code;
  if (type_name == "float" || type_name == "real") {
    type_code = KEYTYPE_FLOAT;
  } else if (type_name == "int" || type_name == "long") {
    type_code = KEYTYPE_INT;
  } else if (type_name == "bool" || type_name == "flag") {
    type_code = KEYTYPE_BOOL;
  } else if (type_name == "file" || type_name == "filein" || type_name == "fileout") {
    type_code = KEYTYPE_FILE;
  } else {
    type_code = KEYTYPE_STRING;
  }

  Keytype_s keytype {
          type_code,
          0, // dimension: single_value
          isoptional, // optional
          nullptr,
          {nullptr}
  };
  return PropertyFactory::createProperty(name, &keytype, default_value, {});
}
