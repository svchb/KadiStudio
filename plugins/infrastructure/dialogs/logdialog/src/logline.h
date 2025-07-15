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

#include <string>

class LogContent;

struct LogLine {
  virtual ~LogLine() = default;
  virtual std::string toString() const = 0;
  virtual void append(const std::string& string) = 0;
  virtual char getLastChar() const = 0;
  virtual bool isEmpty() const = 0;
  virtual const std::string& getContext() const = 0;
};

class LogLineString : public LogLine {
  public:
    explicit LogLineString(const LogContent& parent, const std::string& string) : parent(parent), log_content(string) {}
    std::string toString() const override;
    void append(const std::string& string) override;
    char getLastChar() const override;
    bool isEmpty() const override;
    const std::string& getContext() const override;
  private:
    const LogContent &parent;
    std::string log_content;
};

struct LogLineRef : public LogLine {
  public:
    explicit LogLineRef(LogLine& ref) : ref(ref) {
    }
    std::string toString() const override {
      return ref.toString();
    }
    void append(const std::string& string) override {
      ref.append(string);
    }
    char getLastChar() const override {
      return ref.getLastChar();
    }
    bool isEmpty() const override {
      return ref.isEmpty();
    }
    const std::string& getContext() const override {
      return ref.getContext();
    }
  private:
    LogLine& ref;
};
