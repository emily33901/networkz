#pragma once

#include "common/platform.hh"

#include <string>

// Wrapper for filesystem operations
// This is needed so that we can get files from urls and similar
// (which we probably will be doing most of the time)
namespace filesystem {
bool Init();

void Frame();

// Async file operations

using FileId = u32;

Option<FileId>      Lookup(const std::string &path);
FileId              Request(const std::string &path);
Option<std::string> Contents(FileId f);
void                Dispose(FileId f);

} // namespace filesystem