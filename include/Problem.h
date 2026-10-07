#pragma once

#include <vector>
#include <memory>
#include "State.h"
#include "Package.h"

struct Problem {
    std::vector<Package> allPackages;
    int truckCapacity;
    double alpha;
    double beta;

    Problem(std::vector<Package> pkgs, int cap, double a, double b)
        : allPackages(pkgs), truckCapacity(cap), alpha(a), beta(b) {}

    State getInitialState() const;
    bool isGoal(const State& state) const;
    std::vector<State> getSuccessors(const State& state) const;
    double calculateCost(const State& state) const;
};
