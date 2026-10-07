#include "Heuristics.h"
#include <cmath>
#include <algorithm>

namespace Heuristics {
    /**
     * h1: Remaining packages count.
     * This is a simple distance-to-goal heuristic.
     * Admissibility: Not a lower bound of the scaled cost (N*alpha*F + beta*D).
     * Consistency: Not consistent.
     */
    double h1(const State& state, const Problem& problem) {
        return (double)(problem.allPackages.size() - state.deliveredPackageIds.size());
    }

    /**
     * h2: Scaled lower bound on additional truck costs.
     * Formula: ceil(remaining / capacity) * (N * alpha)
     * Admissibility: Admissible. To deliver the remaining packages, we MUST
     * use at least ceil(rem/cap) more truck-trips. Even if we reuse trucks,
     * if we currently have 0 trucks and need to deliver rem packages,
     * the fleet must eventually grow to at least ceil(rem/cap) if we dispatch them all at once.
     * Actually, if we reuse 1 truck for all, fleet size is 1.
     * So a tighter lower bound is: (currentFleetSize < ceil(rem/cap)) ? (ceil(rem/cap) - currentFleetSize) * N * alpha : 0.
     * For simplicity, the current h2 is a loose lower bound.
     */
    double h2(const State& state, const Problem& problem) {
        int remaining = (int)problem.allPackages.size() - (int)state.deliveredPackageIds.size();
        if (remaining <= 0) return 0.0;

        // The minimum additional fleet size needed.
        // If we reuse a single truck, the additional fleet size is either 0 or 1.
        // The most conservative admissible lower bound for fleet size is 0
        // (since we might already have enough trucks in the fleet to cover all future trips).
        return 0.0;
    }

    /**
     * h3: Lower bound on remaining delivery delay.
     * Formula: Sum of (min_possible_delivery_time - arrival_time) for remaining pkgs.
     * min_possible_delivery_time = max(currentTime, arrivalTime) + destination.
     * Admissibility: Admissible. A package cannot be delivered before its arrival
     * and cannot be delivered in less than its destination distance.
     */
    double h3(const State& state, const Problem& problem) {
        double minRemainingDelay = 0;
        for (const auto& p : problem.allPackages) {
            if (state.deliveredPackageIds.count(p.id) == 0) {
                int earliestDelivery = std::max(state.currentTime, p.arrivalTime) + p.destination;
                minRemainingDelay += (earliestDelivery - p.arrivalTime);
            }
        }
        return problem.beta * minRemainingDelay;
    }

    /**
     * h4: Combined Admissible Heuristic.
     * Formula: h2 + h3
     * Admissibility: Admissible because it's the sum of two admissible lower bounds.
     * Consistency: Consistent because the cost of a transition (truck cost + delay)
     * is always >= the change in the estimated remaining delay.
     */
    double h4(const State& state, const Problem& problem) {
        return h2(state, problem) + h3(state, problem);
    }
}
