#pragma once

#include <vector>
#include <set>
#include <memory>
#include <string>
#include "Package.h"
#include "Truck.h"

struct State {
    int currentTime;
    std::set<int> deliveredPackageIds;
    std::vector<Package> waitingPackages;
    std::vector<Truck> trucks;
    int fleetSize;

    double gCost;
    std::shared_ptr<State> parent;
    std::string action;

    State() : currentTime(0), fleetSize(0), gCost(0.0), parent(nullptr) {}

    bool operator==(const State& other) const;
    bool operator<(const State& other) const;
    std::string getSignature() const;
};

// Custom hasher for State to be used in unordered_set/map
struct StateHasher {
    std::size_t operator()(const State& s) const {
        return std::hash<std::string>{}(s.getSignature());
    }
};
