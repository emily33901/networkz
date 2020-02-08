#pragma once

// Top level node definition

#include <string>

#include <vector>

namespace node {
class Attribute {
public:
    std::string key;
    std::string value;
};

class Node {
public:
    std::vector<Attribute> attributes;

    // What the object is and where we loaded it from
    std::string objectType;
    std::string objectFile;

    // What files were inherited or similar
    std::vector<std::string> inherited;

    virtual void Frame();
};

} // namespace node
