#include "Metrics.h"
#include <algorithm>
#include <numeric>

void Metrics::calculateFinalMetrics(const State& goalState, const Problem& problem, SearchResult& result) {
    int fleetSize = goalState.fleetSize;
    double totalDelay = 0;
    int pkgCount = problem.allPackages.size();

    for (const auto& truck : goalState.trucks) {
        for (const auto& p : truck.loadedPackages) {
            int deliveryTime = truck.dispatchTime + p.destination;
            totalDelay += (deliveryTime - p.arrivalTime);
        }
    }

    double avgDeliveryTime = (pkgCount > 0) ? (totalDelay / pkgCount) : 0;

    result.trucksUsed = fleetSize;
    result.avgDeliveryTime = avgDeliveryTime;
    result.totalCost = (problem.alpha * fleetSize) + (problem.beta * avgDeliveryTime);
}
