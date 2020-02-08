#include "filesystem.hh"

#include <emscripten.h>

#include <atomic>
#include <unordered_map>

namespace filesystem {

bool Init() {
    return true;
}

void Frame() {
}

// TODO mutexes for these maps
// And also maybe semaphore for sync operations

static std::atomic<u32>                                            lastId;
static std::unordered_map<u32, std::pair<std::string, FileLoadFn>> callbackMap;

void Request(std::string path, FileLoadFn callback) {
    auto fileId = ++lastId;

    callbackMap[fileId] = std::make_pair(path, callback);

#if platform_emscripten()
    emscripten_async_wget_data(path.c_str(), (void *)fileId, [](void *arg, void *buffer, int size) {
        auto fileId           = (u32)arg;
        auto [path, callback] = callbackMap[fileId];

        auto text = std::string{(const char *)buffer, (u32)size};

        callback(path, text);

        callbackMap.erase(fileId); }, [](void *arg) {
        auto fileId           = (u32)arg;
        auto [path, callback] = callbackMap[fileId];
        callback(path, None()); });
#endif
}

} // namespace filesystem