#pragma once

#include "State.h"
#include "Problem.h"
#include "SearchResult.h"

class Metrics {
public:
    static void calculateFinalMetrics(const State& goalState, const Problem& problem, SearchResult& result);
};
