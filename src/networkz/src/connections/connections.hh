#pragma once

#include "common/platform.hh"
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
    std::map<u32, uptr>          idMap;
    std::map<u32, connectionLoc> connectionMap;

    Option<uptr> NodeLookup(u32 index);
    Option<u32>  IdLookup(uptr ptr);
    Option<u32>  AddNode(std::pair<u32, uptr> nodeToAdd);
    Option<bool> RemoveNode(u32 index);
    Option<bool> RemoveNode(uptr nodeToRemove);

    Option<u32> CheckForConnection(u32 index1, u32 index2);
    Option<u32> CheckForConnection(uptr node1, uptr node2);
};
} // namespace workspace