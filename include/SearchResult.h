#pragma once

#include <vector>
#include <string>
#include <chrono>
#include <memory>
#include "State.h"

struct SearchResult {
    bool solutionFound;
    double totalCost;
    int trucksUsed;
    double avgDeliveryTime;
    int nodesExpanded;
    int nodesGenerated;
    long long executionTimeMs;
    std::vector<std::string> path;
    std::shared_ptr<State> goalState;

    SearchResult() : solutionFound(false), totalCost(0), trucksUsed(0),
                      avgDeliveryTime(0), nodesExpanded(0), nodesGenerated(0),
                      executionTimeMs(0), goalState(nullptr) {}
};
