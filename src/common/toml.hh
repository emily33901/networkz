#pragma once

#include <cpptoml.h>

namespace cpptoml {
/**
 * Utility function to parse a file as a TOML file. Returns the root table.
 * Throws a parse_exception if the file cannot be opened.
 */
inline std::shared_ptr<table> parse_text(const std::string &text) {
    auto   stream = std::stringstream{text};
    parser p{stream};
    return p.parse();
}
} // namespace cpptoml