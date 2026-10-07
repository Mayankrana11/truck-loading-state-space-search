#include "SearchAlgorithms.h"
#include "Heuristics.h"
#include "Metrics.h"
#include "Utils.h"
#include <vector>
#include <unordered_set>
#include <algorithm>
#include <chrono>

using namespace std;

SearchResult SearchAlgorithms::DFS(const Problem& problem) {
    auto startT = chrono::steady_clock::now();
    SearchResult result;
    State initialState = problem.getInitialState();

    vector<shared_ptr<State>> stack;
    unordered_set<State, StateHasher> visited;

    auto startState = make_shared<State>(initialState);
    stack.push_back(startState);
    result.nodesGenerated++;

    int nodesExpanded = 0;

    while (!stack.empty()) {
        auto current = stack.back();
        stack.pop_back();

        if (problem.isGoal(*current)) {
            auto endT = chrono::steady_clock::now();
            result.solutionFound = true;
            result.path = Utils::reconstructPath(current);
            result.nodesExpanded = nodesExpanded;
            result.executionTimeMs = chrono::duration_cast<chrono::milliseconds>(endT - startT).count();
            result.goalState = current;
            Metrics::calculateFinalMetrics(*current, problem, result);
            return result;
        }

        if (visited.count(*current)) continue;
        visited.insert(*current);
        nodesExpanded++;

        for (auto& nextState : problem.getSuccessors(*current)) {
            auto next = make_shared<State>(nextState);
            next->parent = current;
            if (visited.find(*next) == visited.end()) {
                stack.push_back(next);
                result.nodesGenerated++;
            }
        }
    }

    auto endT = chrono::steady_clock::now();
    result.executionTimeMs = chrono::duration_cast<chrono::milliseconds>(endT - startT).count();
    result.nodesExpanded = nodesExpanded;
    return result;
}
