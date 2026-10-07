#pragma once

#include <functional>
#include "Problem.h"
#include "SearchResult.h"
#include "State.h"

using HeuristicFn = std::function<double(const State&, const Problem&)>;

class SearchAlgorithms {
public:
    static SearchResult BFS(const Problem& problem);
    static SearchResult DFS(const Problem& problem);
    static SearchResult UCS(const Problem& problem);
    static SearchResult Greedy(const Problem& problem, HeuristicFn h);
    static SearchResult AStar(const Problem& problem, HeuristicFn h);
    static SearchResult BeamSearch(const Problem& problem, HeuristicFn h, int beamWidth);
};
