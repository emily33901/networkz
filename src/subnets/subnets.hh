#pragma once

#include "common/platform.hh"
#include "connections/connections.hh"
#include <vector>

namespace subnet {
class Subnet {
public:
    //TODO
    //Convert to using Option<Pin> instead of connectors
    /*
    std::vector<Pin> inPins;
    std::vector<Pin> outPins;
*/
    std::vector<connections::Connector> inConnectors;
    std::vector<connections::Connector> outConnectors;

    std::vector<uptr> internalComponents;

    u16 subnetLevel;

    //Constructor Method
    // (Default)
    Subnet();
    // (Specialised)
    Subnet(std::vector<connections::Connector> inC, std::vector<connections::Connector> outC, std::vector<uptr> interns, u16 subnetLvl);

    /*
    Option<std::vector<Pin>> ListInPins();
    Option<std::vector<Pin>> ListOutPins();
    
    */

    Option<std::vector<uptr>> ListInputs();
    Option<std::vector<uptr>> ListOutputs();
};
} // namespace subnet