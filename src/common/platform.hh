#pragma once

#include "defer.hh"

#include <cstdint>
#include <optional>
#include <typeinfo>

// Nicer types for the basic integers

using u8   = std::uint8_t;
using u16  = std::uint16_t;
using u32  = std::uint32_t;
using u64  = std::uint64_t;
using uptr = std::uintptr_t;

using i8   = std::int8_t;
using i16  = std::int16_t;
using i32  = std::int32_t;
using i64  = std::int64_t;
using iptr = std::intptr_t;

#if defined(ARGONX_WIN)
#define AssertImpl(exp, message, ...)                                      \
    if (!(exp)) {                                                          \
        printf(__FILE__ ":%d: " #message "\n", __LINE__ - 1, __VA_ARGS__); \
        assert(0);                                                         \
        abort();                                                           \
    }
#elif defined(ARGONX_UNIX)
#define AssertImpl(exp, message, ...)                                        \
    if (!!!(exp)) {                                                          \
        _Pragma("GCC diagnostic push");                                      \
        _Pragma("GCC diagnostic ignored \"-Wformat\"");                      \
        printf(__FILE__ ":%d: " #message "\n", __LINE__ - 1, ##__VA_ARGS__); \
        assert(0);                                                           \
        abort();                                                             \
        _Pragma("GCC diagnostic pop");                                       \
    }
#endif

#if defined(_DEBUG)
#define Assert AssertImpl
#define AssertAlways AssertImpl
#else
#define Assert(exp, message, ...) assert(exp)
#define AssertAlways AssertImpl
#endif

#define Macro_ConcatenateDetail(x, y) x##y
#define Macro_Concatenate(x, y) Macro_ConcatenateDetail(x, y)

#define Macro_ConcatenateDetail(x, y) x##y
#define Macro_Concatenate(x, y) Macro_ConcatenateDetail(x, y)

#ifdef __EMSCRIPTEN__
#define platform_emscripten() 1
#else
#define platform_emscripten() 0
#endif

// Nicer keywords for optional
#define Option std::optional
#define None std::nullopt
#define Some(v) std::make_optional(v)

template <typename std::optional<typename T>>
using OptionalInternal = T;

#define MatchOption(var, onSome, onNone) \
    if (var.has_value()) {               \
        const auto &var = *var;          \
        onSome;                          \
    } else {                             \
        onNone;                          \
    }                                    \
    }
