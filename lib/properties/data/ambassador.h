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

#pragma once

#include <set>
#include <string>
#include <vector>

#include "valuetypeinterface/valuetypeinterface.h"
#include "valuetypeinterfacehint.h"
#include "property.h"
#include "properties/propertychangelistener.h"
#include "properties/functionproperty.h"

class PersistentManager;

/**
 * @brief      The Ambassador listens on controls and delegates the
 *             changes to the model so that it can update it's data.
 * @ingroup    data
 */
class Ambassador : public Property, public PropertyChangeListener, public ValueTypeInterface<Ambassador> {

  public:
    static constexpr std::string_view EVENT_DOM_CHANGE{"dom_change"};

    /**
    * @param modelname The name of the model.
    * @param name The name of the property.
    */
    Ambassador(const std::string& modelname, const std::string& name);
    Ambassador(const std::string& modelname)
      : Ambassador(modelname, modelname) {
    }

    Ambassador(Ambassador const&) = delete;
    Ambassador& operator=(Ambassador const&) = delete;

    virtual ~Ambassador();


    AbstractValueTypeInterface* getValueTypeInterface() override {
      return this;
    }

    const AbstractValueTypeInterface* getValueTypeInterface() const override {
      return this;
    }

    bool compareToString(const std::string& /*cmp_string*/) const override {
      return false;
    }

    Ambassador const & getValue() const override {
      return *this;
    }

    using Property::getValueTypeInfo;

    /**
      * @brief  Gets the name of this model
      */
    const std::string& getModelName() const {
      return modelname;
    }

    /**
     * Gets the property value as a string.
     *
     * @param name The property name.
     * @return The value as a string.
     */
    void fromString(const std::string& /*str*/) override {
      // value_type value;
      // fromStringInternal(str, value);
      // ValueTypeInterface<T>::setValue(value);
    }

    std::string toString() const override {
      return "";//toStringInternal(getValue());
    }

    /** Stores the content of the ambassador with help of a persistentmanager
     *
     * @param persistentmanager Instance of the PersistentManager interface
     * @param keynamespace      namespace wich should be used as a prefix
     */
    void store(PersistentManager* persistentmanager, const std::string& keynamespace) const override;

    void load(PersistentManager* persistentmanager, const std::string& keynamespace) override;

    /**
     * @brief Traverse the property tree up until the root is found.
     *
     * @return Ambassador* The root ambassador.
     */
    Ambassador* getRootAmbassador() const;

    /**
     * @brief Traverse the property tree to find a property.
     *
     * @param path The path to traverse. Can contain '..'.
     * If the path begins with an '/' the traversal of the tree starts at the root.
     * @return Property* The found property or nullptr if no property to the given path exists.
     *
     * @throws invalid_argument if the path did not lead to a property
     */
    Property* traversePath(const std::string& path) const {
      auto start_from_root = (!path.empty() && path.front() == '/');
      if (start_from_root) {
        return getRootAmbassador()->traversePath(cpputils::split(path, "/"));
      } else {
        return traversePath(cpputils::split(path, "/"));
      }
    }

    /**
      * @brief  gets the value of a named property in the collection
      *
      * @param name The name of the property
      *
      * @return The wanted property.
      */
    Property* getProperty(const std::string& path) const {
      return traversePath(cpputils::split(path, "/"));
    }

    /// getProperty<T> must be called with T=Ambassador*, not T=const Ambassador*
    template<typename TProperty>
    TProperty* getProperty(const std::string& name) const {
      static_assert(
        std::is_base_of<Property, TProperty>::value,
        "TProperty must be a descendant of Property"
      );

      TProperty *tproperty = dynamic_cast<TProperty*>(getProperty(name));
#ifdef DEBUG
      if (tproperty == nullptr) {
        throw std::runtime_error("Property '" + name + "' is not of type " + typeid(TProperty).name() + ".");
      }
#endif

      return tproperty;
    }

    bool hasProperty(const std::string& path) const;

    const std::vector<std::unique_ptr<Property>>& getProperties() const {
      return properties;
    }

    AbstractValueTypeInterface* getValueTypeInterface(const std::string& name) {
      return getProperty(name)->getValueTypeInterface();
    }

    void registerPropertyChangeListener(PropertyChangeListener* listener);
    void removePropertyChangeListener(PropertyChangeListener* listener);

    void callUpdateFunction(const std::string& name) {
      getProperty<FunctionProperty>(name)->callUpdateFunction();
      setDirty(true);
    }

    template <typename T>
    void setValueWithPath(const std::vector<std::string>& path, const T& value) {
      getVTI<T>(traversePath(path))->setValue(value);
    }

    template <typename T>
    void setValue(const std::string& path, const T& value) {
      setValueWithPath(cpputils::split(path, "/"), value);
    }

    template <typename T>
    const T& getValueWithPath(const std::vector<std::string>& path) const {
      return getVTI<T>(traversePath(path))->getValue();
    }

    template <typename T>
    const T& getValue(const std::string& path) const {
      return getValueWithPath<T>(cpputils::split(path, "/"));
    }

    template <typename T>
    void getValueWithPath(const std::vector<std::string>& path, T& value) const {
      getVTI<T>(traversePath(path))->getValue(value);
    }

    template <typename T>
    void getValue(const std::string& path, T& value) const {
      getValueWithPath(cpputils::split(path, "/"), value);
    }

    const ValueTypeInterfaceHint* getHint(const std::string& propertyname) const {
      return getProperty(propertyname)->getHint();
    }

    ValueTypeInterfaceHint* updateHint(const std::string& propertyname) {
      return getProperty(propertyname)->updateHint();
    }

    using ValueTypeInterface<Ambassador>::updateHint;
    using ValueTypeInterface<Ambassador>::getHint;

    /**
      * @brief Check whether this model needs an update or not
      *
      * @return true if the model needs an update
      */
    bool isDirty() const {
      return is_dirty;
    }

    void clean() {
      event_bus.publish(EVENT_DOM_CHANGE, std::string());
    }

    void dump(int level = 0) const;

    EventHandles& getEventHandles() {
      return event_handles;
    }

    const EventBus& getEvents() const {
      return event_bus;
    }

    template<typename F>
    [[nodiscard]] AbstractSignal::Handle onDomChange(F&& callback) {
      return event_bus.subscribe<std::string>(EVENT_DOM_CHANGE, callback);
    }

  class SuspendScope final {
    public:
      SuspendScope(Ambassador *parent) : parent{parent} {
        ++parent->suspend_count;
      }

      ~SuspendScope() {
        --parent->suspend_count;
        parent->commit();
      }

      private:
        Ambassador *parent;
    };

  SuspendScope suspend() {
    return SuspendScope(this);
  }


  [[nodiscard]] bool isSuspended() const { return suspend_count > 0; }

  protected:
    void commit() {
      if (suspend_count <= 0) {
        event_bus.publish(EVENT_DOM_CHANGE, std::string());
        setDirty(false);

        suspend_count = 0;
      }
    }

    /// inform the listeners about a value change
    void notifyPropertyChange(const Property* property);

  private:
    template <typename T>
    ValueTypeInterface<T>* getVTI(Property* property) const {
      auto vti = dynamic_cast<ValueTypeInterface<T>*>(property->getValueTypeInterface());
      if (vti == nullptr) {
        throw std::runtime_error("Invalid VTI for Property '" + property->getName() + "' with type '" + property->getValueTypeInfo().name() + "' is not valid for '" + typeid(T).name() + "'.");
      }
      return vti;
    }

    Property* getProp(const std::string& propertyname) const {
      for (const auto &property : getProperties()) {
        if (property->hasName(propertyname)) {
          return property.get();
        }
      }
#ifdef DEBUG
      dump();
#endif
      throw std::runtime_error("Property '" + propertyname + "' not found.");
    }

    /**
     * @brief Traverse a path to a specific Property. If a property was found the pointer to the property is returned.
     *
     * @param path The path to traverse. Can contain '..'
     * @return Property* The found property or nullptr if no property to the given path exists.
     *
     * @throws invalid_argument if the path did not lead to a property
     */
    Property* traversePath(const std::vector<std::string>& path) const;

    std::set<PropertyChangeListener*> listeners;

    friend Property;
    template <typename TVTI>
    friend class PropertyVTI;

    int suspend_count{};

  protected:
    void setValueRaw(Ambassador const /*value*/) override {
      // TODO
      throw std::runtime_error("ValueType::setValueRaw can not change value for type '" + std::string(getValueTypeInfo().name()) + "'.");
    }

    /**
      * @brief Set the "needs-update" - state of this model
      */
    void setDirty(bool dirty) {
      is_dirty = dirty;
    }

    std::vector<std::unique_ptr<Property>> properties;
    std::string modelname;

    bool is_dirty;
};
