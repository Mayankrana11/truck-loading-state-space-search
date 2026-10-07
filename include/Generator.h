#pragma once

#include <vector>
#include <string>
#include "Package.h"

class Generator {
public:
    static std::vector<Package> generatePackages(int count, int destRange, int timeRange, int seed);
};
