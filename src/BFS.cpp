#include "SearchAlgorithms.h"
#include "Heuristics.h"
#include "Metrics.h"
#include "Utils.h"
#include <queue>
#include <unordered_set>
#include <algorithm>
#include <chrono>

using namespace std;

SearchResult SearchAlgorithms::BFS(const Problem& problem) {
    auto startT = chrono::steady_clock::now();
    SearchResult result;
    State initialState = problem.getInitialState();

    queue<shared_ptr<State>> openList;
    unordered_set<State, StateHasher> closedList;

    auto startState = make_shared<State>(initialState);
    openList.push(startState);
    result.nodesGenerated++;

    int nodesExpanded = 0;

    while (!openList.empty()) {
        auto current = openList.front();
        openList.pop();

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

        if (closedList.count(*current)) continue;
        closedList.insert(*current);
        nodesExpanded++;

        for (auto& nextState : problem.getSuccessors(*current)) {
            auto next = make_shared<State>(nextState);
            next->parent = current;
            if (closedList.find(*next) == closedList.end()) {
                openList.push(next);
                result.nodesGenerated++;
            }
        }
    }

    auto endT = chrono::steady_clock::now();
    result.executionTimeMs = chrono::duration_cast<chrono::milliseconds>(endT - startT).count();
    result.nodesExpanded = nodesExpanded;
    return result;
}
