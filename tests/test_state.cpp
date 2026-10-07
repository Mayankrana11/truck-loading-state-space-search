#include <iostream>
#include <vector>
#include <cmath>
#include <cassert>
#include "State.h"
#include "Package.h"
#include "Truck.h"

// Simple Test Framework
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

void test_TC_S1_InitialState() {
    State s;
    bool ok = (s.currentTime == 0) &&
              (s.fleetSize == 0) &&
              (s.gCost == 0.0) &&
              (s.deliveredPackageIds.empty()) &&
              (s.trucks.empty()) &&
              (s.waitingPackages.empty());
    report(ok, "TC-S1: Initial/default state");
}

void test_TC_S2_FieldUpdates() {
    State s;
    s.currentTime = 10;
    s.fleetSize = 2;
    s.gCost = 15.5;
    bool ok = (s.currentTime == 10) && (s.fleetSize == 2) && (std::abs(s.gCost - 15.5) < 1e-6);
    report(ok, "TC-S2: State field updates");
}

void test_TC_S3_PackageIdsAffectEquality() {
    State s1, s2;
    s1.deliveredPackageIds.insert(1);
    s2.deliveredPackageIds.insert(2);
    report(!(s1 == s2), "TC-S3: Package IDs affect equality");
}

void test_TC_S4_CurrentTimeAffectsEquality() {
    State s1, s2;
    s1.currentTime = 5;
    s2.currentTime = 10;
    report(!(s1 == s2), "TC-S4: Current time affects equality");
}

void test_TC_S5_TruckInfoAffectsEquality() {
    State s1, s2;
    Truck t1, t2;
    t1.id = 1; t1.dispatchTime = 0; t1.returnTime = 10;
    t2.id = 1; t2.dispatchTime = 0; t2.returnTime = 20;
    s1.trucks.push_back(t1);
    s2.trucks.push_back(t2);
    report(!(s1 == s2), "TC-S5: Truck information affects equality");
}

void test_TC_S6_FleetSizeAffectsEquality() {
    State s1, s2;
    s1.fleetSize = 1;
    s2.fleetSize = 2;
    report(!(s1 == s2), "TC-S6: Fleet size affects equality");
}

void test_TC_S7_IdenticalStatesEqual() {
    State s1, s2;
    s1.currentTime = 10;
    s1.fleetSize = 1;
    s1.deliveredPackageIds.insert(1);

    s2.currentTime = 10;
    s2.fleetSize = 1;
    s2.deliveredPackageIds.insert(1);

    report(s1 == s2, "TC-S7: Identical states compare equal");
}

void test_TC_S8_gCostDoesNotAffectIdentity() {
    State s1, s2;
    s1.currentTime = 10;
    s1.gCost = 100.0;

    s2.currentTime = 10;
    s2.gCost = 200.0;

    report(s1 == s2, "TC-S8: gCost does NOT affect state identity");
}

void test_TC_S9_SignatureConsistency() {
    State s1, s2;
    s1.currentTime = 10;
    s1.fleetSize = 1;

    s2.currentTime = 10;
    s2.fleetSize = 1;

    report(s1.getSignature() == s2.getSignature(), "TC-S9: Signature/hash consistency");
}

int main() {
    std::cout << "Running State Tests..." << std::endl;
    test_TC_S1_InitialState();
    test_TC_S2_FieldUpdates();
    test_TC_S3_PackageIdsAffectEquality();
    test_TC_S4_CurrentTimeAffectsEquality();
    test_TC_S5_TruckInfoAffectsEquality();
    test_TC_S6_FleetSizeAffectsEquality();
    test_TC_S7_IdenticalStatesEqual();
    test_TC_S8_gCostDoesNotAffectIdentity();
    test_TC_S9_SignatureConsistency();

    std::cout << "\n====================================" << std::endl;
    std::cout << "TEST SUMMARY" << std::endl;
    std::cout << "Passed: " << passed << std::endl;
    std::cout << "Failed: " << failed << std::endl;
    std::cout << "====================================" << std::endl;

    return (failed == 0) ? 0 : 1;
}
