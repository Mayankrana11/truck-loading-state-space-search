#include <iostream>
#include <vector>
#include <cmath>
#include "Heuristics.h"
#include "State.h"
#include "Problem.h"
#include "Package.h"

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

void test_TC_H1_GoalState() {
    std::vector<Package> pkgs = {{0, 0, 5}};
    Problem prob(pkgs, 1, 10.0, 1.0);
    State s;
    s.deliveredPackageIds.insert(0);

    bool ok = (Heuristics::h1(s, prob) == 0) &&
              (Heuristics::h2(s, prob) == 0) &&
              (Heuristics::h3(s, prob) == 0) &&
              (Heuristics::h4(s, prob) == 0);
    report(ok, "TC-H1: Goal state heuristics are 0");
}

void test_TC_H2_NonNegativity() {
    std::vector<Package> pkgs = {{0, 0, 5}, {1, 0, 10}};
    Problem prob(pkgs, 2, 10.0, 1.0);
    State s;
    s.currentTime = 0;

    bool ok = (Heuristics::h1(s, prob) >= 0) &&
              (Heuristics::h2(s, prob) >= 0) &&
              (Heuristics::h3(s, prob) >= 0) &&
              (Heuristics::h4(s, prob) >= 0);
    report(ok, "TC-H2: Non-negativity");
}

void test_TC_H3_h1Calculation() {
    std::vector<Package> pkgs = {{0, 0, 5}, {1, 0, 5}, {2, 0, 5}, {3, 0, 5}, {4, 0, 5}};
    Problem prob(pkgs, 2, 10.0, 1.0);
    State s;
    s.deliveredPackageIds.insert(0);
    s.deliveredPackageIds.insert(1);

    report(Heuristics::h1(s, prob) == 3, "TC-H3: h1 remaining package count");
}

void test_TC_H4_h2Calculation() {
    std::vector<Package> pkgs = {{0, 0, 5}, {1, 0, 10}};
    Problem prob(pkgs, 2, 10.0, 1.0);
    State s;

    report(Heuristics::h2(s, prob) == 0, "TC-H4: h2 conservative bound is 0");
}

void test_TC_H5_h3KnownCalculation() {
    // currentTime = 10
    // P0: arrival=5, destination=3 -> max(10, 5)+3-5 = 8
    // P1: arrival=12, destination=4 -> max(10, 12)+4-12 = 4
    // beta = 2.0
    // h3 = 2.0 * (8 + 4) = 24.0

    std::vector<Package> pkgs = {{0, 5, 3}, {1, 12, 4}};
    Problem prob(pkgs, 2, 10.0, 2.0);
    State s;
    s.currentTime = 10;

    double val = Heuristics::h3(s, prob);
    report(std::abs(val - 24.0) < 1e-6, "TC-H5: h3 known calculation");
}

void test_TC_H6_h3FutureArriving() {
    // currentTime = 5, arrival = 10, destination = 3
    // max(5, 10)+3-10 = 3
    std::vector<Package> pkgs = {{0, 10, 3}};
    Problem prob(pkgs, 1, 10.0, 1.0);
    State s;
    s.currentTime = 5;

    double val = Heuristics::h3(s, prob);
    report(val >= 0 && std::abs(val - 3.0) < 1e-6, "TC-H6: h3 future arrival");
}

void test_TC_H7_h4Combination() {
    std::vector<Package> pkgs = {{0, 0, 5}};
    Problem prob(pkgs, 1, 10.0, 1.0);
    State s;

    double h2 = Heuristics::h2(s, prob);
    double h3 = Heuristics::h3(s, prob);
    double h4 = Heuristics::h4(s, prob);

    report(std::abs(h4 - (h2 + h3)) < 1e-6, "TC-H7: h4 == h2 + h3");
}

void test_TC_H8_BetaScaling() {
    std::vector<Package> pkgs = {{0, 0, 5}};
    Problem prob1(pkgs, 1, 10.0, 0.1);
    Problem prob2(pkgs, 1, 10.0, 1.0);
    Problem prob3(pkgs, 1, 10.0, 10.0);
    State s;

    double v1 = Heuristics::h3(s, prob1);
    double v2 = Heuristics::h3(s, prob2);
    double v3 = Heuristics::h3(s, prob3);

    report(std::abs(v1 * 10.0 - v2) < 1e-6 && std::abs(v2 * 10.0 - v3) < 1e-6, "TC-H8: h3 scales linearly with beta");
}

int main() {
    std::cout << "Running Heuristics Tests..." << std::endl;
    test_TC_H1_GoalState();
    test_TC_H2_NonNegativity();
    test_TC_H3_h1Calculation();
    test_TC_H4_h2Calculation();
    test_TC_H5_h3KnownCalculation();
    test_TC_H6_h3FutureArriving();
    test_TC_H7_h4Combination();
    test_TC_H8_BetaScaling();

    std::cout << "\n====================================" << std::endl;
    std::cout << "TEST SUMMARY" << std::endl;
    std::cout << "Passed: " << passed << std::endl;
    std::cout << "Failed: " << failed << std::endl;
    std::cout << "====================================" << std::endl;

    return (failed == 0) ? 0 : 1;
}
