#include <iostream>
#include <vector>
#include <set>
#include <algorithm>
#include <cmath>
#include <iomanip>
#include <unordered_map>
#include "Problem.h"
#include "State.h"
#include "SearchAlgorithms.h"
#include "Heuristics.h"
#include "SearchResult.h"
#include "Utils.h"

struct BruteForceResult {
    bool found = false;
    double minCost = 1e18;
};

// Brute force explores all possible loading decisions and dispatch times
void solveRecursive(const Problem& prob, State current, BruteForceResult& best) {
    if (prob.isGoal(current)) {
        double totalDelay = 0;
        for (const auto& truck : current.trucks) {
            for (const auto& p : truck.loadedPackages) {
                totalDelay += (truck.dispatchTime + p.destination - p.arrivalTime);
            }
        }
        // Original Objective: alpha * fleet + beta * (totalDelay / N)
        double cost = prob.alpha * current.fleetSize + prob.beta * (totalDelay / prob.allPackages.size());

        if (cost < best.minCost) {
            best.minCost = cost;
            best.found = true;
        }
        return;
    }

    std::vector<State> succs = prob.getSuccessors(current);
    for (const auto& next : succs) {
        solveRecursive(prob, next, best);
    }
}

// Helper to get the exact remaining cost from a state using brute force
double getExactRemainingCost(const Problem& prob, State state) {
    BruteForceResult bf;
    solveRecursive(prob, state, bf);

    // The brute force calculates the TOTAL cost.
    // To get remaining cost, subtract the current cost.
    // Note: Current cost is scaled in search, but BF uses unscaled.
    // This is tricky. Let's just use the unscaled BF and subtract state.gCost/N.

    // A better way: The BF result is the total final cost.
    // The remaining cost = FinalCost - CurrentCost.
    // Current cost = prob.alpha * state.fleetSize + prob.beta * (delay_so_far / N)

    double delaySoFar = 0;
    for (const auto& truck : state.trucks) {
        for (const auto& p : truck.loadedPackages) {
            delaySoFar += (truck.dispatchTime + p.destination - p.arrivalTime);
        }
    }
    double currentCost = prob.alpha * state.fleetSize + prob.beta * (delaySoFar / prob.allPackages.size());

    return bf.minCost - currentCost;
}

void runOptimalityTest(int numPkgs, int cap, double alpha, double beta) {
    std::vector<Package> pkgs;
    for (int i = 0; i < numPkgs; ++i) {
        pkgs.push_back({i, i * 2, (i + 1) * 2});
    }
    Problem prob(pkgs, cap, alpha, beta);

    BruteForceResult bf;
    solveRecursive(prob, prob.getInitialState(), bf);

    auto resUCS = SearchAlgorithms::UCS(prob);
    auto resAStar = SearchAlgorithms::AStar(prob, Heuristics::h4);

    std::cout << "N=" << numPkgs << " C=" << cap << " a=" << alpha << " b=" << beta
              << " | BF: " << bf.minCost << " UCS: " << (resUCS.solutionFound ? std::to_string(resUCS.totalCost) : "NF")
              << " A*: " << (resAStar.solutionFound ? std::to_string(resAStar.totalCost) : "NF") << "\n";

    if (resUCS.solutionFound && std::abs(resUCS.totalCost - bf.minCost) < 1e-6) {
        std::cout << "  [PASS] UCS optimal\n";
    } else {
        std::cout << "  [FAIL] UCS not optimal\n";
    }

    if (resAStar.solutionFound && std::abs(resAStar.totalCost - bf.minCost) < 1e-6) {
        std::cout << "  [PASS] A* optimal\n";
    } else {
        std::cout << "  [FAIL] A* not optimal\n";
    }
}

void testHeuristicProperties(int numPkgs, int cap, double alpha, double beta) {
    std::vector<Package> pkgs;
    for (int i = 0; i < numPkgs; ++i) {
        pkgs.push_back({i, i * 2, (i + 1) * 2});
    }
    Problem prob(pkgs, cap, alpha, beta);

    int admPass = 0, admFail = 0;
    int conPass = 0, conFail = 0;
    int statesTested = 0;

    // We'll use BFS to explore some reachable states
    std::vector<State> visited;
    std::vector<State> queue;
    queue.push_back(prob.getInitialState());
    std::set<std::string> seen;
    seen.insert(queue[0].getSignature());

    int head = 0;
    while(head < queue.size() && queue.size() < 500) {
        State s = queue[head++];
        statesTested++;

        // Admissibility check
        double h = Heuristics::h4(s, prob);
        double exact = getExactRemainingCost(prob, s);
        // We check h(s) <= exact * N because h is scaled for gCost (Scaled = N * Cost)
        if (h <= (exact * prob.allPackages.size()) + 1e-6) {
            admPass++;
        } else {
            admFail++;
        }

        // Consistency check
        std::vector<State> succs = prob.getSuccessors(s);
        for (const auto& next : succs) {
            double h_s = Heuristics::h4(s, prob);
            double h_next = Heuristics::h4(next, prob);
            double cost_s_next = next.gCost - s.gCost;
            if (h_s <= cost_s_next + h_next + 1e-6) {
                conPass++;
            } else {
                conFail++;
            }

            if (seen.find(next.getSignature()) == seen.end()) {
                seen.insert(next.getSignature());
                queue.push_back(next);
            }
        }
    }

    std::cout << "Heuristic h4 Validation (N=" << numPkgs << ", a=" << alpha << ", b=" << beta << "):\n";
    std::cout << "  Admissibility: " << admPass << " PASS, " << admFail << " FAIL\n";
    std::cout << "  Consistency: " << conPass << " PASS, " << conFail << " FAIL\n";
    std::cout << "  States Tested: " << statesTested << "\n";
}

int main() {
    std::cout << "=== FINAL OPTIMALITY & HEURISTIC AUDIT ===\n";

    std::vector<double> alphas = {0.5, 10.0, 100.0};
    std::vector<double> betas = {0.1, 0.5, 1.0, 2.0, 10.0};

    for (double a : alphas) {
        for (double b : betas) {
            runOptimalityTest(3, 2, a, b);
        }
    }

    std::cout << "\n--- Heuristic Empirical Tests ---\n";
    testHeuristicProperties(3, 2, 10.0, 1.0);
    testHeuristicProperties(3, 2, 100.0, 0.1);

    return 0;
}
