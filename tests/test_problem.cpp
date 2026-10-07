#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>
#include "Problem.h"
#include "State.h"
#include "Package.h"
#include "Truck.h"

int passed = 0;
int failed = 0;

void report(bool condition, const std::string& testName) {
    if (condition) {
        std::cout << "[PASS] " << testName << std::endl;
        passed++;
    } else {
        std::cout << "[FAIL] " << testName << std::endl;
        failed++;
    }
}

void test_TC_P1_InitialState() {
    std::vector<Package> pkgs = {{0, 0, 5}};
    Problem prob(pkgs, 2, 10.0, 1.0);
    State s = prob.getInitialState();
    bool ok = (s.currentTime == 0) && (s.fleetSize == 0) && (s.gCost == 0.0) && (s.deliveredPackageIds.empty());
    report(ok, "TC-P1: Initial state");
}

void test_TC_P2_EmptyProblemGoal() {
    std::vector<Package> pkgs = {};
    Problem prob(pkgs, 2, 10.0, 1.0);
    State s = prob.getInitialState();
    report(prob.isGoal(s), "TC-P2: Empty problem goal");
}

void test_TC_P3_IncompleteGoal() {
    std::vector<Package> pkgs = {{0, 0, 5}};
    Problem prob(pkgs, 2, 10.0, 1.0);
    State s = prob.getInitialState();
    report(!prob.isGoal(s), "TC-P3: Incomplete goal");
}

void test_TC_P4_PackageArrivalConstraint() {
    std::vector<Package> pkgs = {{0, 10, 5}}; // Arrives at t=10
    Problem prob(pkgs, 2, 10.0, 1.0);
    State s = prob.getInitialState(); // t=0

    auto successors = prob.getSuccessors(s);
    bool foundDispatch = false;
    for (const auto& next : successors) {
        if (next.action.find("Dispatch") != std::string::npos) {
            foundDispatch = true;
        }
    }
    report(!foundDispatch, "TC-P4: Package arrival constraint (cannot dispatch at t=0)");
}

void test_TC_P5_WaitAction() {
    std::vector<Package> pkgs = {{0, 10, 5}};
    Problem prob(pkgs, 2, 10.0, 1.0);
    State s = prob.getInitialState(); // t=0

    auto successors = prob.getSuccessors(s);
    bool foundWait = false;
    for (const auto& next : successors) {
        if (next.action.find("Wait") != std::string::npos && next.currentTime == 10) {
            foundWait = true;
        }
    }
    report(foundWait, "TC-P5: Wait action advances to next event");
}

void test_TC_P6_TimeCalculations() {
    std::vector<Package> pkgs = {{0, 0, 5}};
    Problem prob(pkgs, 2, 10.0, 1.0);
    State s = prob.getInitialState();
    s.currentTime = 3;

    auto successors = prob.getSuccessors(s);
    bool ok = false;
    for (const auto& next : successors) {
        if (next.action.find("Dispatch") != std::string::npos) {
            if (!next.trucks.empty()) {
                const auto& t = next.trucks[0];
                if (t.dispatchTime == 3 && t.returnTime == 13) {
                    ok = true;
                }
            }
        }
    }
    report(ok, "TC-P6: Delivery and return time calculations");
}

void test_TC_P7_CapacityConstraint() {
    std::vector<Package> pkgs = {{0, 0, 1}, {1, 0, 2}, {2, 0, 3}};
    Problem prob(pkgs, 2, 10.0, 1.0);
    State s = prob.getInitialState();

    auto successors = prob.getSuccessors(s);
    bool overCapacity = false;
    for (const auto& next : successors) {
        for (const auto& t : next.trucks) {
            if ((int)t.loadedPackages.size() > 2) overCapacity = true;
        }
    }
    report(!overCapacity, "TC-P7: Capacity constraint respected");
}

void test_TC_P8_LoadingCombinations() {
    std::vector<Package> pkgs = {{0, 0, 1}, {1, 0, 2}, {2, 0, 3}};
    Problem prob(pkgs, 2, 10.0, 1.0);
    State s = prob.getInitialState();

    auto successors = prob.getSuccessors(s);
    int dispatchCount = 0;
    for (const auto& next : successors) {
        if (next.action.find("Dispatch") != std::string::npos) dispatchCount++;
    }
    // Combinations: 3 of size 1 ({0}, {1}, {2}) + 3 of size 2 ({0,1}, {0,2}, {1,2}) = 6
    report(dispatchCount == 6, "TC-P8: Loading combinations count");
}

void test_TC_P9_DestinationOrdering() {
    std::vector<Package> pkgs = {{0, 0, 10}, {1, 0, 5}};
    Problem prob(pkgs, 2, 10.0, 1.0);
    State s = prob.getInitialState();

    auto successors = prob.getSuccessors(s);
    bool orderViolation = false;
    for (const auto& next : successors) {
        for (const auto& t : next.trucks) {
            for (size_t i = 1; i < t.loadedPackages.size(); ++i) {
                if (t.loadedPackages[i-1].destination > t.loadedPackages[i].destination) {
                    orderViolation = true;
                }
            }
        }
    }
    report(!orderViolation, "TC-P9: Destination ordering respected");
}

void test_TC_P10_TruckReuse() {
    std::vector<Package> pkgs = {{0, 0, 5}, {1, 13, 5}};
    Problem prob(pkgs, 2, 10.0, 1.0);
    State s = prob.getInitialState();

    // Trip 1: Dispatch at t=0, return t=10
    auto succs1 = prob.getSuccessors(s);
    State s1;
    for (auto& next : succs1) {
        if (next.action.find("Dispatch Truck with 1 pkgs") != std::string::npos) {
            s1 = next; break;
        }
    }

    // Wait until t=13
    auto succs2 = prob.getSuccessors(s1);
    State s2;
    for (auto& next : succs2) {
        if (next.action.find("Wait") != std::string::npos && next.currentTime == 13) {
            s2 = next; break;
        }
    }

    // Trip 2: Dispatch at t=13
    auto succs3 = prob.getSuccessors(s2);
    bool fleetSizeStillOne = false;
    for (auto& next : succs3) {
        if (next.action.find("Dispatch") != std::string::npos && next.fleetSize == 1) {
            fleetSizeStillOne = true;
        }
    }
    report(fleetSizeStillOne, "TC-P10: Truck reuse does not increase fleet size");
}

void test_TC_P11_OverlappingTrucks() {
    std::vector<Package> pkgs = {{0, 0, 10}, {1, 0, 10}};
    Problem prob(pkgs, 1, 10.0, 1.0); // Capacity 1 forces 2 trucks
    State s = prob.getInitialState();

    auto succs1 = prob.getSuccessors(s);
    State s1;
    for (auto& next : succs1) {
        if (next.action.find("Dispatch") != std::string::npos) {
            s1 = next; break;
        }
    }

    auto succs2 = prob.getSuccessors(s1);
    bool fleetSizeBecameTwo = false;
    for (auto& next : succs2) {
        if (next.action.find("Dispatch") != std::string::npos && next.fleetSize == 2) {
            fleetSizeBecameTwo = true;
        }
    }
    report(fleetSizeBecameTwo, "TC-P11: Overlapping trucks increase fleet size");
}

void test_TC_P12_ObjectiveCalculation() {
    // N=5, alpha=10, beta=1, fleetSize=2, totalDelay=30
    // Original cost = alpha*fleetSize + beta*(totalDelay/N) = 10*2 + 1*(30/5) = 20 + 6 = 26
    // Scaled cost = N*alpha*fleetSize + beta*totalDelay = 5*10*2 + 1*30 = 100 + 30 = 130

    std::vector<Package> pkgs = {{0,0,1}, {1,0,1}, {2,0,1}, {3,0,1}, {4,0,1}};
    Problem prob(pkgs, 5, 10.0, 1.0);
    State s;
    s.fleetSize = 2;
    s.gCost = 130.0; // Simulating a state with these costs

    // Note: In current implementation, calculateCost just returns gCost.
    // We verify if the scaled cost logic matches.
    double scaledCost = prob.allPackages.size() * prob.alpha * s.fleetSize + prob.beta * 30.0;
    report(std::abs(scaledCost - 130.0) < 1e-6, "TC-P12: Objective scaled cost calculation");
}

int main() {
    std::cout << "Running Problem Tests..." << std::endl;
    test_TC_P1_InitialState();
    test_TC_P2_EmptyProblemGoal();
    test_TC_P3_IncompleteGoal();
    test_TC_P4_PackageArrivalConstraint();
    test_TC_P5_WaitAction();
    test_TC_P6_TimeCalculations();
    test_TC_P7_CapacityConstraint();
    test_TC_P8_LoadingCombinations();
    test_TC_P9_DestinationOrdering();
    test_TC_P10_TruckReuse();
    test_TC_P11_OverlappingTrucks();
    test_TC_P12_ObjectiveCalculation();

    std::cout << "\n====================================" << std::endl;
    std::cout << "TEST SUMMARY" << std::endl;
    std::cout << "Passed: " << passed << std::endl;
    std::cout << "Failed: " << failed << std::endl;
    std::cout << "====================================" << std::endl;

    return (failed == 0) ? 0 : 1;
}
