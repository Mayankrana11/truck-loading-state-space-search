#include "State.h"
#include <sstream>
#include <algorithm>

bool State::operator==(const State& other) const {
    if (currentTime != other.currentTime) return false;
    if (deliveredPackageIds != other.deliveredPackageIds) return false;
    if (waitingPackages.size() != other.waitingPackages.size()) return false;

    // Check waiting packages (order might vary, but logically they are the same set)
    // For simplicity in this problem, we assume waitingPackages is maintained sorted by ID
    for (size_t i = 0; i < waitingPackages.size(); ++i) {
        if (waitingPackages[i].id != other.waitingPackages[i].id) return false;
    }

    if (trucks.size() != other.trucks.size()) return false;
    if (fleetSize != other.fleetSize) return false;
    for (size_t i = 0; i < trucks.size(); ++i) {
        if (trucks[i].id != other.trucks[i].id) return false;
        if (trucks[i].dispatchTime != other.trucks[i].dispatchTime) return false;
        if (trucks[i].returnTime != other.trucks[i].returnTime) return false;
        if (trucks[i].loadedPackages.size() != other.trucks[i].loadedPackages.size()) return false;
        for (size_t j = 0; j < trucks[i].loadedPackages.size(); ++j) {
            if (trucks[i].loadedPackages[j].id != other.trucks[i].loadedPackages[j].id) return false;
        }
    }
    return true;
}

bool State::operator<(const State& other) const {
    return getSignature() < other.getSignature();
}

std::string State::getSignature() const {
    std::stringstream ss;
    ss << "t:" << currentTime << "|f:" << fleetSize << "|d:";
    for (int id : deliveredPackageIds) ss << id << ",";
    ss << "|w:";
    for (const auto& p : waitingPackages) ss << p.id << ",";
    ss << "|tr:";
    for (const auto& t : trucks) {
        ss << t.id << "(" << t.dispatchTime << "," << t.returnTime << ")";
        for (const auto& p : t.loadedPackages) ss << p.id << ",";
        ss << ";";
    }
    return ss.str();
}
