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

/**
 * @file    slot.h
 * @ingroup cpputils
 * @brief   Provides signals and slots which can be used as a
 *          callback mechanism.
 */

#pragma once

#include <functional>
#include <utility>
#include <map>
#include <memory>

/// @brief Base for Signal class.
class AbstractSignal : public std::enable_shared_from_this<AbstractSignal> {
  public:

   /// @brief Represents a registered callback.
   /// Automatically unregisters from the parent signal.
   class Handle final {
      friend AbstractSignal;

      public:
        /// @brief Default CTOR.
        Handle() = default;

        Handle(const Handle&) = delete;

        Handle(Handle&& other) noexcept {
          *this = std::move(other);
        }

        /// @brief Destroyes the handle and unregisters from the parent.
        ~Handle() {
          if (auto ptr = parent.lock()) {
            ptr->disconnect(std::move(*this));
          }
        }

        Handle& operator=(const Handle&) = delete;

        Handle& operator=(Handle&& other) {
          parent = std::move(other.parent);
          id = std::exchange(other.id, 0);

          return *this;
        }

        /// @brief Return the ID of the callback.
        /// @return The ID.
        size_t getID() const noexcept {
          return id;
        }

        const AbstractSignal* getParent() const {
          return parent.lock().get();
        }

        /// @brief Releases the handle from the parent.
        /// The callback is then registered for the complete lifetime of the signal.
        void release() {
          parent.reset();
        }

      private:
        /// @brief Create a handle.
        /// @param id The ID of the callback.
        /// @param parent The parent where the callback is registered.
        Handle(size_t id, std::weak_ptr<AbstractSignal> parent) noexcept : id{id}, parent{parent} {}

        size_t id{};
        std::weak_ptr<AbstractSignal> parent{};
    };

  public:
    virtual ~AbstractSignal() = default;

  protected:
    Handle createHandle(size_t id) {
      return Handle(id, this->shared_from_this());
    }

    /// @brief Unregisters a callback.
    /// @param handle The handle of the callback.
    virtual void disconnect(Handle&& handle) = 0;
};

/// @brief Allows to register callbacks.
/// @tparam T The type of value to notify about.
template<typename T>
class Signal : public AbstractSignal {
  public:
    using Slot = std::function<void(const T&)>;

    virtual ~Signal() = default;

    /// @brief Notify all registered callbacks about a change.
    /// @param value The new value.
    void notify(const T& value) const {
      for (auto it = slot_queue.begin(); it != slot_queue.end(); ++it) {
        it->second(value);
      }
    }

    /// @brief Register a callback.
    /// The callback must be valid for the entire lifetime of the signal instance.
    /// @param slot The callback.
    void connect(Slot&& slot) {
      slot_queue.insert({++id_counter, slot});
    }

    /// @brief Register a callback.
    /// Callback is automatically unregistered if the returned handle runs out of scope.
    /// @param slot The callback.
    /// @return The handle to the callback.
    Handle connectScoped(Slot&& slot) {
      const auto id = ++id_counter;
      slot_queue.insert({id, slot});

      return createHandle(id);
    }

    /// @brief Compares two signal instances.
    /// @param rhs The other signal
    /// @return True if the signals instances are equal.
    bool operator==(const Signal<T>& rhs) const {
      return slot_queue == rhs.slot_queue;
    }

  protected:
    /// @brief Unregisters a callback.
    /// @param handle The handle of the callback.
    void disconnect(Handle&& handle) override {
      slot_queue.erase(handle.getID());
    }

  private:
    size_t id_counter{};
    std::map<size_t, Slot> slot_queue{};
};
