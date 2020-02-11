#pragma once

// Top level node definition

#include <string>
#include <vector>

#include "common/platform.hh"

#include "common/nodeeditor.hh"

namespace node {

class Node;

namespace factory {
// Code that manages possible nodes and how to create them
bool Init();

Option<Node *> Create(const std::string &name);
Option<Node *> Inherit(Node *node, const std::string &name);
void           Destroy(Node *n);

} // namespace factory

class Attribute {
public:
    std::string key;

    // TODO this value is going to need to be slightly more complex
    std::string value;
};

class Node {
public:
    Node();
    virtual ~Node();

    // TODO figure out if this needs to actually be virtual
    virtual Node *Clone();

    ed::NodeId id;

    std::vector<Attribute> attributes;

    // What the object is and where we loaded it from
    std::string name;
    std::string desc;
    std::string file;

    // What files were inherited or similar
    std::vector<std::string> inherited;

    std::unordered_map<std::string, ed::PinId>;

    // For rendering
    virtual void Frame();

    // For thinking
    virtual void Think();
};

} // namespace node
