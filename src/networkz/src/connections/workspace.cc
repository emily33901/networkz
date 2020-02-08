#include "connections.hh"

namespace workspace {
Option<uptr> Workspace::NodeLookup(u32 index) {
    if (idMap.find(index) == idMap.end) {
        return None();
    } else {
        return Some(idMap.at(index));
    }
}

Option<u32> Workspace::IdLookup(uptr ptr) {
    bool flag = false;
    u32  id;
    for (auto x : idMap) {
        if (x.second == ptr) {
            flag = true;
            id   = x.first;
            break;
        }
    }
    if (flag) {
        return Some(id);
    } else {
        return None();
    }
}

Option<u32> Workspace::AddNode(std::pair<u32, uptr> nodeToAdd) {
    auto responsePair = idMap.insert(nodeToAdd);

    if (responsePair.second) {
        return Some(nodeToAdd.first);
    } else {
        return None();
    }
}

Option<bool> Workspace::RemoveNode(u32 index) {
    if (idMap.find(index) == idMap.end) {
        return None();
    } else {
        idMap.erase(index);
        return Some(true);
    }
}

Option<bool> Workspace::RemoveNode(uptr nodeToRemove) {
    auto nodeId = IdLookup(nodeToRemove);

    if (nodeId == None()) {
        return None();
    } else {
        idMap.erase(*nodeId);
        return Some(true);
    }
}

Option<u32> Workspace::CheckForConnection(u32 index1, u32 index2) {
    bool flag     = false;
    u32  returnId = 0;
    for (std::pair<u32, connectionLoc> x : connectionMap) {
        connectionLoc currConnection = x.second;
        if (currConnection->connectedNodes == std::pair<u32, u32>(index1, index2)) {
            flag     = true;
            returnId = currConnection->connectionId;
            break;
        }
        if (currConnection->connectedNodes == std::pair<u32, u32>(index2, index1)) {
            flag     = true;
            returnId = currConnection->connectionId;
            break;
        }
    }

    if (flag) {
        return Some(returnId);
    } else {
        return None();
    }
}

Option<u32> Workspace::CheckForConnection(uptr node1, uptr node2) {
    bool flag     = false;
    u32  returnId = 0;
    u32  index1   = *IdLookup(node1);
    u32  index2   = *IdLookup(node2);
    for (auto x : connectionMap) {
        connectionLoc currConnection = x.second;
        if (currConnection->connectedNodes == std::pair<u32, u32>(index1, index2)) {
            flag     = true;
            returnId = currConnection->connectionId;
            break;
        }
        if (currConnection->connectedNodes == std::pair<u32, u32>(index2, index1)) {
            flag     = true;
            returnId = currConnection->connectionId;
            break;
        }
    }

    if (flag) {
        return Some(returnId);
    } else {
        return None();
    }
}

} // namespace workspace