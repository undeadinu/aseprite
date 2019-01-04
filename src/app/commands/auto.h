// Aseprite
// Copyright (C) 2019  Igara Studio S.A.
//
// This program is distributed under the terms of
// the End-User License Agreement for Aseprite.

#ifndef APP_COMMANDS_AUTO_H
#define APP_COMMANDS_AUTO_H

#include "app/commands/command.h"
#include "app/commands/params.h"

#include <vector>

namespace app {

class AutoLoadParamsCommand : public Command {
public:
  AutoLoadParamsCommand(const char* id, CommandFlags flags)
    : Command(id, flags) {
  }

  obs::signal<void(const Params& params)> LoadParams;

private:
  void onLoadParams(const Params& params) override {
    LoadParams(params);
  }
};

template<typename T>
class AutoParam {
public:
  AutoParam(AutoLoadParamsCommand* command,
            const T& defaultValue,
            const char* key)
    : m_defaultValue(defaultValue) {
    if (key)
      m_keys.push_back(key);

    command->LoadParams.connect(
      [this](const Params& params){ onLoadParams(params); });
  }

  AutoParam(AutoLoadParamsCommand* command,
            const T& defaultValue,
            const std::initializer_list<const char*> keys)
    : AutoParam(command, defaultValue, nullptr) {
    for (const auto& key : keys)
      m_keys.push_back(key);
  }

  const T& operator()() const {
    return m_value;
  }

  void operator()(const T& newValue) {
    m_value = newValue;
  }

private:
  void onLoadParams(const Params& params) {
    m_value = m_defaultValue;
    for (auto key : m_keys) {
      if (params.has_param(key)) {
        m_value = params.get_as<T>(key);
        break;
      }
    }
  }

  T m_defaultValue;
  T m_value;
  std::vector<const char*> m_keys;
};

} // namespace app

#endif
