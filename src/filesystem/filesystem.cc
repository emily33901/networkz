#include "filesystem.hh"

#include <emscripten.h>

#include <atomic>
#include <unordered_map>

namespace filesystem {

#ifdef __EMSCRIPTEN__
// clang-format off
EM_JS(std::string, FetchFile, (std::string url, FileId id), {
    fetch(url).then(x => x.text(), r => {

    }).then(x => Module.ccall(
        "FilesystemFileResolved", "void", ["number", "string"], [id, url] 
    ));
});
// clang-format on
#endif

bool Init() {
    return true;
}

void Frame() {
}

// TODO mutexes for these maps
// And also maybe semaphore for sync operations

static std::atomic<FileId>                     lastId;
static std::unordered_map<FileId, std::string> fileMap;
static std::unordered_map<std::string, FileId> urlMap;

Option<FileId> Lookup(std::string &url) {
    auto r = urlMap.find(url);

    if (r != urlMap.end()) return Some(r->second);

    return None();
}

FileId Request(const std::string &path) {
    auto fileId = ++lastId;

    FetchFile(path, fileId);

    urlMap[path] = fileId;

    return fileId;
}

Option<std::string> Contents(FileId id) {
    auto r = fileMap.find(id);

    auto v = *r;

    if (r != fileMap.end()) return Some(r->second);

    return None();
}

void Dispose(FileId f) {
    fileMap.erase(f);
    std::string url;
    for (auto x : urlMap) {
        if (x.second == f) url = x.first;
    }
    urlMap.erase(url);
}

} // namespace filesystem

extern "C" void FilesystemFileResolved(filesystem::FileId id, std::string text) {
    filesystem::fileMap[id] = text;
}