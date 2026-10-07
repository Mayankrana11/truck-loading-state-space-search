#pragma once

#include "State.h"
#include "Problem.h"

namespace Heuristics {
    double h1(const State& state, const Problem& problem); // Remaining packages
    double h2(const State& state, const Problem& problem); // Lower bound trucks
    double h3(const State& state, const Problem& problem); // Est delivery delay
    double h4(const State& state, const Problem& problem); // Combined
}
