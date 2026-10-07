#pragma once

#include <vector>
#include "Package.h"

struct Truck {
    int id;
    int capacity;
    std::vector<Package> loadedPackages;
    int dispatchTime = -1;
    int returnTime = -1;

    bool isAvailable(int currentTime) const {
        return returnTime == -1 || currentTime >= returnTime;
    }
};
