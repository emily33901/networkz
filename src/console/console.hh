#pragma once

#include <functional>

namespace console {
bool Init();
void Frame();

void Log(const char *fmt, ...);

// TODO split args
using Callback = std::function<void(const std::string &args)>;

void Register(const std::string &command, Callback cb);

void Execute(const std::string &command);

} // namespace console