#include "Generator.h"
#include <random>

std::vector<Package> Generator::generatePackages(int count, int destRange, int timeRange, int seed) {
    std::vector<Package> pkgs;
    std::mt19937 gen(seed);
    std::uniform_int_distribution<> distDest(1, destRange);
    std::uniform_int_distribution<> distTime(0, timeRange);

    for (int i = 0; i < count; ++i) {
        pkgs.push_back({i, distTime(gen), distDest(gen)});
    }
    return pkgs;
}
