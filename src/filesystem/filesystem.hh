#pragma once

#include "common/platform.hh"

#include <functional>
#include <string>

// Wrapper for filesystem operations
// This is needed so that we can get files from urls and similar
// (which we probably will be doing most of the time)
namespace filesystem {
bool Init();

void Frame();

// Async file operations

// TODO probably std::function this
using FileLoadFn = std::function<void(const std::string &path, Option<std::string> buffer)>;

void Request(std::string path, FileLoadFn callback);

} // namespace filesystem