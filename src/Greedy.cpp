#include "SearchAlgorithms.h"
#include "Heuristics.h"
#include "Metrics.h"
#include "Utils.h"
#include <queue>
#include <unordered_set>
#include <algorithm>
#include <chrono>

using namespace std;

SearchResult SearchAlgorithms::Greedy(const Problem& problem, HeuristicFn h) {
    auto startT = chrono::steady_clock::now();
    SearchResult result;
    State initialState = problem.getInitialState();

    using Node = pair<double, shared_ptr<State>>;
    priority_queue<Node, vector<Node>, greater<Node>> openList;
    unordered_set<State, StateHasher> closedList;

    auto startState = make_shared<State>(initialState);
    openList.push({h(*startState, problem), startState});
    result.nodesGenerated++;

    int nodesExpanded = 0;

    while (!openList.empty()) {
        auto node = openList.top();
        double hVal = node.first;
        auto current = node.second;
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
                openList.push({h(*next, problem), next});
                result.nodesGenerated++;
            }
        }
    }

    auto endT = chrono::steady_clock::now();
    result.executionTimeMs = chrono::duration_cast<chrono::milliseconds>(endT - startT).count();
    result.nodesExpanded = nodesExpanded;
    return result;
}
