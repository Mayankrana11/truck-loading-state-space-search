#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>
#include "SearchAlgorithms.h"
#include "Problem.h"
#include "Package.h"
#include "Heuristics.h"
#include "SearchResult.h"

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

bool verifySolution(const SearchResult& res, const Problem& prob) {
    if (!res.solutionFound) return false;
    if (res.goalState == nullptr) return false;

    // 1. Every package delivered
    if (res.goalState->deliveredPackageIds.size() != prob.allPackages.size()) return false;

    // 2. Basic metrics positive
    if (res.nodesExpanded < 0 || res.nodesGenerated < 0 || res.executionTimeMs < 0) return false;
    if (res.totalCost < 0) return false;

    return true;
}

void test_TC_A1_TrivialProblem() {
    std::vector<Package> pkgs = {{0, 0, 5}};
    Problem prob(pkgs, 1, 10.0, 1.0);

    auto resBFS = SearchAlgorithms::BFS(prob);
    auto resDFS = SearchAlgorithms::DFS(prob);
    auto resUCS = SearchAlgorithms::UCS(prob);
    auto resGreedy = SearchAlgorithms::Greedy(prob, Heuristics::h4);
    auto resAStar = SearchAlgorithms::AStar(prob, Heuristics::h4);
    auto resBeam = SearchAlgorithms::BeamSearch(prob, Heuristics::h4, 2);

    bool ok = resBFS.solutionFound && resDFS.solutionFound && resUCS.solutionFound &&
              resGreedy.solutionFound && resAStar.solutionFound && resBeam.solutionFound;
    report(ok, "TC-A1: Trivial problem solved by all");
}

void test_TC_A2_SmallProblem() {
    std::vector<Package> pkgs = {{0, 0, 5}, {1, 2, 10}, {2, 5, 15}};
    Problem prob(pkgs, 2, 10.0, 1.0);

    auto resBFS = SearchAlgorithms::BFS(prob);
    auto resDFS = SearchAlgorithms::DFS(prob);
    auto resUCS = SearchAlgorithms::UCS(prob);
    auto resGreedy = SearchAlgorithms::Greedy(prob, Heuristics::h4);
    auto resAStar = SearchAlgorithms::AStar(prob, Heuristics::h4);
    auto resBeam = SearchAlgorithms::BeamSearch(prob, Heuristics::h4, 2);

    bool allValid = verifySolution(resBFS, prob) && verifySolution(resDFS, prob) &&
                    verifySolution(resUCS, prob) && verifySolution(resGreedy, prob) &&
                    verifySolution(resAStar, prob) && verifySolution(resBeam, prob);
    report(allValid, "TC-A2: Small problem results are valid");
}

void test_TC_A3_UCSvsAStar() {
    std::vector<Package> pkgs = {{0, 0, 5}, {1, 2, 10}, {2, 5, 15}};
    Problem prob(pkgs, 2, 10.0, 1.0);

    auto resUCS = SearchAlgorithms::UCS(prob);
    auto resAStar = SearchAlgorithms::AStar(prob, Heuristics::h4);

    report(std::abs(resUCS.totalCost - resAStar.totalCost) < 1e-6, "TC-A3: UCS cost == A* cost");
}

void test_TC_A5_MetricsSanity() {
    std::vector<Package> pkgs = {{0, 0, 5}};
    Problem prob(pkgs, 1, 10.0, 1.0);
    auto res = SearchAlgorithms::AStar(prob, Heuristics::h4);

    bool ok = (res.nodesExpanded >= 0) && (res.nodesGenerated >= 0) && (res.executionTimeMs >= 0);
    report(ok, "TC-A5: Search metrics sanity");
}

void test_TC_A6_EmptyProblem() {
    std::vector<Package> pkgs = {};
    Problem prob(pkgs, 2, 10.0, 1.0);

    auto resUCS = SearchAlgorithms::UCS(prob);
    report(resUCS.solutionFound && resUCS.totalCost == 0, "TC-A6: Empty problem cost 0");
}

void test_TC_A7_CapacityOne() {
    std::vector<Package> pkgs = {{0, 0, 5}, {1, 0, 10}, {2, 0, 15}};
    Problem prob(pkgs, 1, 10.0, 1.0);

    auto resUCS = SearchAlgorithms::UCS(prob);
    report(verifySolution(resUCS, prob), "TC-A7: Capacity=1 solved");
}

void test_TC_A8_CapacityLarge() {
    std::vector<Package> pkgs = {{0, 0, 5}, {1, 0, 10}, {2, 0, 15}};
    Problem prob(pkgs, 10, 10.0, 1.0);

    auto resUCS = SearchAlgorithms::UCS(prob);
    report(verifySolution(resUCS, prob), "TC-A8: Capacity >= N solved");
}

int main() {
    std::cout << "Running Search Tests..." << std::endl;
    test_TC_A1_TrivialProblem();
    test_TC_A2_SmallProblem();
    test_TC_A3_UCSvsAStar();
    test_TC_A5_MetricsSanity();
    test_TC_A6_EmptyProblem();
    test_TC_A7_CapacityOne();
    test_TC_A8_CapacityLarge();

    std::cout << "\n====================================" << std::endl;
    std::cout << "TEST SUMMARY" << std::endl;
    std::cout << "Passed: " << passed << std::endl;
    std::cout << "Failed: " << failed << std::endl;
    std::cout << "====================================" << std::endl;

    return (failed == 0) ? 0 : 1;
}
