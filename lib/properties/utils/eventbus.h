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

#include "slot.h"

#include <algorithm>
#include <map>
#include <string_view>
#include <vector>

/// @brief Provides a pub/sub mechanism for events.
class EventBus final {
  public:
    using Events = std::map<std::string_view, std::shared_ptr<Signal<const void*>>>;

  public:
    EventBus() = default;
    EventBus(const EventBus&) = delete;
    EventBus(EventBus&&) = default;
    ~EventBus() = default;

    EventBus& operator=(const EventBus&) = delete;
    EventBus& operator=(EventBus&&) = default;

    /// @brief Subscribe to a specific event.
    /// @tparam T The type of callback parameter.
    /// @tparam F The type of the callback.
    /// @param event The event.
    /// @param callback The callback.
    /// @return Handle to the subscription.
    template<typename T, typename F>
    [[nodiscard]] AbstractSignal::Handle subscribe(std::string_view event, F&& callback) {
      addEvent(event);

      return events.at(event)->connectScoped([callback](const void* data) {
        if constexpr (std::is_void_v<T>) {
          callback();
        } else {
          callback(*reinterpret_cast<const T*>(data));
        }
      });
    }

    /// @brief Publish an event with data.
    /// @tparam T The type of the data.
    /// @param event The event.
    /// @param data The data to send.
    template<typename T>
    void publish(std::string_view event, const T& data) {
      if (events.find(event) != events.end()) {
        events.at(event)->notify(reinterpret_cast<const void*>(&data));
      }
    }

    /// @brief Publish an event without data.
    /// @param event The event.
    void publish(std::string_view event) {
      if (events.find(event) != events.end()) {
        events.at(event)->notify(nullptr);
      }
    }

    /// @brief Returns all events.
    /// @return The events.
    [[nodiscard]] const Events& getEvents() const {
      return events;
    }

  private:
    void addEvent(std::string_view event) {
      // TODO: Check if event is allowed.
      if (events.find(event) == events.end()) {
        events.insert({event, std::make_shared<Signal<const void*>>()});
      }
    }

  private:
    Events events{};
};

/// @brief Keeps track of event handles.
class EventHandles final {
  public:
    EventHandles() = default;
    EventHandles(const EventHandles&) = delete;
    EventHandles(EventHandles&&) = default;
    ~EventHandles() = default;

    EventHandles& operator=(const EventHandles&) = delete;
    EventHandles& operator=(EventHandles&&) = default;

    /// @brief Track a handle.
    /// @param handle The handle.
    void track(AbstractSignal::Handle&& handle) {
      handles.push_back(std::move(handle));
    }

    /// @brief Untrack a handle.
    /// @param event The handle.
    void untrack(const AbstractSignal* event) {
      if (event == nullptr) {
        return;
      }

      handles.erase(
        std::remove_if(handles.begin(), handles.end(), [&event](const auto& handle) {
          return handle.getParent() == event;
        }),
        handles.end()
      );
    }

    /// @brief Removes all event handles.
    void clear() {
      handles.clear();
    }

    /// @brief Remove all dangling handles.
    void cleanUp() {
      handles.erase(
        std::remove_if(handles.begin(), handles.end(), [&](const auto& handle) {
          return handle.getParent() == nullptr;
        }),
        handles.end()
      );
    }

    /// @brief Operator to track a handle.
    /// @param handle The handle.
    /// @return All tracked handles.
    EventHandles& operator+=(AbstractSignal::Handle&& handle) {
      track(std::move(handle));
      return *this;
    }

    /// @brief Remove all handles to events from a specific bus.
    /// @param event_bus The event bus.
    /// @return All tracked handles.
    EventHandles& operator-=(const EventBus& event_bus) {
      for (const auto &event : event_bus.getEvents()) {
        untrack(event.second.get());
      }

      return *this;
    }

  private:
    std::vector<AbstractSignal::Handle> handles{};
};
