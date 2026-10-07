#include <iostream>
#include <vector>
#include <iomanip>
#include <chrono>
#include <map>
#include "Generator.h"
#include "Problem.h"
#include "SearchAlgorithms.h"
#include "Heuristics.h"
#include "SearchResult.h"
#include "State.h"
#include "Utils.h"

void printFullComparisonRow(const std::string& algo, const SearchResult& res, const Problem& problem) {
    std::cout << std::left << std::setw(15) << algo
              << std::setw(10) << (res.solutionFound ? "Yes" : "No")
              << std::setw(15) << (res.solutionFound ? std::to_string(res.totalCost) : "N/A")
              << std::setw(15) << (res.solutionFound ? std::to_string(res.trucksUsed) : "N/A")
              << std::setw(15) << (res.solutionFound ? std::to_string(res.avgDeliveryTime * problem.allPackages.size()) : "N/A")
              << std::setw(15) << (res.solutionFound ? std::to_string(res.avgDeliveryTime) : "N/A")
              << std::setw(15) << res.nodesExpanded
              << std::setw(15) << res.nodesGenerated
              << std::setw(15) << res.executionTimeMs
              << std::endl;
}

void printForensicTable(const std::string& algoName, const SearchResult& res, const Problem& problem) {
    std::cout << "\n=== VALIDATION: " << algoName << " ===\n";
    if (!res.solutionFound || res.goalState == nullptr) {
        std::cout << "No solution found or goal state missing.\n";
        return;
    }

    const State& state = *(res.goalState);

    std::cout << "Solution Path:\n";
    for (const auto& step : res.path) {
        std::cout << "  " << step << "\n";
    }

    std::cout << "\nPackage Details:\n";
    std::cout << std::left << std::setw(10) << "PkgID"
              << std::setw(12) << "Arrival"
              << std::setw(12) << "Dispatch"
              << std::setw(10) << "Dest"
              << std::setw(12) << "Delivery"
              << std::setw(10) << "Delay" << "\n";
    std::cout << std::string(66, '-') << "\n";

    double totalDelay = 0;
    for (const auto& truck : state.trucks) {
        for (const auto& p : truck.loadedPackages) {
            int deliveryTime = truck.dispatchTime + p.destination;
            int delay = deliveryTime - p.arrivalTime;
            totalDelay += delay;
            std::cout << std::left << std::setw(10) << p.id
                      << std::setw(12) << p.arrivalTime
                      << std::setw(12) << truck.dispatchTime
                      << std::setw(10) << p.destination
                      << std::setw(12) << deliveryTime
                      << std::setw(10) << delay << "\n";
        }
    }

    double avgDelay = (problem.allPackages.size() > 0) ? (totalDelay / problem.allPackages.size()) : 0;

    std::cout << "\n--- Totals ---\n";
    std::cout << "Fleet Size: " << state.fleetSize << "\n";
    std::cout << "Total Delay: " << totalDelay << "\n";
    std::cout << "Average Delay: " << avgDelay << "\n";
    std::cout << "Truck Cost (alpha * fleet): " << (problem.alpha * state.fleetSize) << "\n";
    std::cout << "Delay Cost (beta * avg): " << (problem.beta * avgDelay) << "\n";
    std::cout << "Independently Calculated Total Cost: " << (problem.alpha * state.fleetSize) + (problem.beta * avgDelay) << "\n";
    std::cout << "Reported Total Cost: " << res.totalCost << "\n";
}

int main() {
    // Deterministic instance
    std::vector<Package> pkgs = {
        {0, 7, 8},
        {1, 19, 2},
        {2, 15, 8},
        {3, 12, 6},
        {4, 3, 5}
    };
    int truckCapacity = 2;
    double alpha = 10.0;
    double beta = 1.0;
    int beamWidth = 3;

    Problem problem(pkgs, truckCapacity, alpha, beta);

    std::cout << "--- Six-Algorithm Comparison Run ---\n";
    std::cout << "Packages: 5, Capacity: " << truckCapacity << ", Alpha: " << alpha << ", Beta: " << beta << "\n\n";

    auto resBFS = SearchAlgorithms::BFS(problem);
    auto resDFS = SearchAlgorithms::DFS(problem);
    auto resUCS = SearchAlgorithms::UCS(problem);
    auto resGreedy = SearchAlgorithms::Greedy(problem, Heuristics::h4);
    auto resAStar = SearchAlgorithms::AStar(problem, Heuristics::h4);
    auto resBeam = SearchAlgorithms::BeamSearch(problem, Heuristics::h4, beamWidth);

    std::cout << std::left << std::setw(15) << "Algorithm"
              << std::setw(10) << "Found?"
              << std::setw(15) << "Total Cost"
              << std::setw(15) << "Fleet Size"
              << std::setw(15) << "Total Delay"
              << std::setw(15) << "Avg Delay"
              << std::setw(15) << "Nodes Exp"
              << std::setw(15) << "Nodes Gen"
              << std::setw(15) << "Time (ms)"
              << std::endl;
    std::cout << std::string(130, '-') << std::endl;

    printFullComparisonRow("BFS", resBFS, problem);
    printFullComparisonRow("DFS", resDFS, problem);
    printFullComparisonRow("UCS", resUCS, problem);
    printFullComparisonRow("Greedy", resGreedy, problem);
    printFullComparisonRow("A*", resAStar, problem);
    printFullComparisonRow("Beam", resBeam, problem);

    std::cout << "\n\n";
    printForensicTable("UCS", resUCS, problem);

    return 0;
}
