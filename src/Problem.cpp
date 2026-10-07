#include "Problem.h"
#include <algorithm>
#include <cmath>
#include <iostream>
#include <vector>

// Helper to generate all combinations of a certain size
void getCombinations(const std::vector<Package>& available, int k, int start, std::vector<Package>& current, std::vector<std::vector<Package>>& result) {
    if ((int)current.size() == k) {
        result.push_back(current);
        return;
    }
    for (int i = start; i < (int)available.size(); ++i) {
        current.push_back(available[i]);
        getCombinations(available, k, i + 1, current, result);
        current.pop_back();
    }
}

State Problem::getInitialState() const {
    State start;
    start.currentTime = 0;
    start.waitingPackages = {};
    start.trucks = {};
    start.fleetSize = 0;
    start.gCost = 0.0;
    return start;
}

bool Problem::isGoal(const State& state) const {
    return state.deliveredPackageIds.size() == allPackages.size();
}

std::vector<State> Problem::getSuccessors(const State& state) const {
    std::vector<State> successors;

    std::vector<Package> available;
    for (const auto& p : allPackages) {
        if (p.arrivalTime <= state.currentTime) {
            if (state.deliveredPackageIds.count(p.id) == 0) {
                bool inTruck = false;
                for (const auto& t : state.trucks) {
                    for (const auto& lp : t.loadedPackages) if (lp.id == p.id) { inTruck = true; break; }
                    if (inTruck) break;
                }
                if (!inTruck) available.push_back(p);
            }
        }
    }

    std::sort(available.begin(), available.end(), [](const Package& a, const Package& b) {
        if (a.destination != b.destination) return a.destination < b.destination;
        return a.id < b.id;
    });

    if (!available.empty()) {
        for (int size = 1; size <= truckCapacity; ++size) {
            std::vector<std::vector<Package>> subsets;
            std::vector<Package> current;
            if (size <= (int)available.size()) {
                getCombinations(available, size, 0, current, subsets);
            } else if (size > (int)available.size() && !available.empty()) {
                // We can load all available packages even if it's less than capacity
                // But this case is already handled by the loop if size == available.size()
            }

            for (const auto& subset : subsets) {
                State next = state;
                Truck t;
                t.id = state.trucks.size() + 1;
                t.capacity = truckCapacity;

                int maxDest = 0;
                double addedDelay = 0;
                for (const auto& p : subset) {
                    t.loadedPackages.push_back(p);
                    maxDest = std::max(maxDest, p.destination);
                    addedDelay += (next.currentTime + p.destination - p.arrivalTime);
                }

                t.dispatchTime = next.currentTime;
                t.returnTime = next.currentTime + (2 * maxDest);
                next.trucks.push_back(t);

                for (const auto& lp : t.loadedPackages) {
                    next.deliveredPackageIds.insert(lp.id);
                }

                int activeTrucks = 0;
                for (const auto& tr : next.trucks) {
                    if (tr.returnTime > next.currentTime) {
                        activeTrucks++;
                    }
                }
                next.fleetSize = std::max(state.fleetSize, activeTrucks);

                next.action = "Dispatch Truck with " + std::to_string(subset.size()) + " pkgs";

                // SCALED OBJECTIVE: N * alpha * FleetSize + beta * TotalDelay
                double truckCostInc = 0;
                if (next.fleetSize > state.fleetSize) {
                    truckCostInc = (double)allPackages.size() * alpha;
                }
                next.gCost = state.gCost + truckCostInc + (beta * addedDelay);
                successors.push_back(next);
            }
        }
    }

    int nextEventTime = 1e9;
    bool eventFound = false;
    for (const auto& p : allPackages) {
        if (p.arrivalTime > state.currentTime) {
            nextEventTime = std::min(nextEventTime, p.arrivalTime);
            eventFound = true;
        }
    }
    for (const auto& t : state.trucks) {
        if (t.returnTime > state.currentTime) {
            nextEventTime = std::min(nextEventTime, t.returnTime);
            eventFound = true;
        }
    }

    if (eventFound) {
        State next = state;
        next.currentTime = nextEventTime;
        next.action = "Wait until " + std::to_string(nextEventTime);
        next.gCost = state.gCost;
        successors.push_back(next);
    }

    return successors;
}

double Problem::calculateCost(const State& state) const {
    return state.gCost;
}
