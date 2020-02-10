#pragma once

#include "common/nodeeditor.hh"
#include "common/platform.hh"
#include "node/node.hh"
#include <map>
#include <string>
#include <utility>

namespace connections {
class Connector {
public:
    u32                 connectionId;
    std::pair<u32, u32> connectedNodes;
    std::string         typeName;
};
} // namespace connections

namespace workspace {
class Workspace {
public:
    using connectionLoc = connections::Connector *;
    std::map<u32, node::Node *>  idMap;
    std::map<u32, connectionLoc> connectionMap;

    Option<node::Node *> NodeLookup(u32 index);
    Option<u32>          IdLookup(node::Node *ptr);
    Option<u32>          AddNode(std::pair<u32, node::Node *> nodeToAdd);
    Option<bool>         RemoveNode(u32 index);
    Option<bool>         RemoveNode(node::Node *nodeToRemove);

    Option<u32> CheckForConnection(u32 index1, u32 index2);
    Option<u32> CheckForConnection(node::Node *node1, node::Node *node2);
};
} // namespace workspace