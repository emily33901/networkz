#include "connections/connections.hh"
#include "subnets.hh"

namespace subnet {
Option<std::vector<uptr>> Subnet::ListInputs() {
    std::vector<u32>     inIDs;
    std::vector<uptr>    inNodes;
    workspace::Workspace currWorkSpace;
    //Add nodes to vector
    for (auto con : inConnectors) {
        inIDs.push_back(con.connectedNodes.first);
    }

    int vecSize = inIDs.size();
    for (int i; i < vecSize; i++) {
        inNodes.push_back(*(currWorkSpace.NodeLookup(inIDs[i])));
    }

    if (inNodes.size != 0) {
        return Some(inNodes);
    } else {
        return None();
    }
}

Option<std::vector<uptr>> Subnet::ListOutputs() {
    std::vector<u32>     outIDs;
    std::vector<uptr>    outNodes;
    workspace::Workspace currWorkSpace;

    for (auto con : inConnectors) {
        outIDs.push_back(con.connectedNodes.second);
    }

    int vecSize = outIDs.size();
    for (int i; i < vecSize; i++) {
        outNodes.push_back(*(currWorkSpace.NodeLookup(outIDs[i])));
    }

    if (outNodes.size != 0) {
        return Some(outNodes);
    } else {
        return None();
    }
}
} // namespace subnet