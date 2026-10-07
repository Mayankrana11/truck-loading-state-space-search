#include "SearchAlgorithms.h"
#include "Heuristics.h"
#include "Metrics.h"
#include "Utils.h"
#include <vector>
#include <unordered_set>
#include <algorithm>
#include <chrono>

using namespace std;

SearchResult SearchAlgorithms::BeamSearch(const Problem& problem, HeuristicFn h, int beamWidth) {
    auto startT = chrono::steady_clock::now();
    SearchResult result;
    State initialState = problem.getInitialState();

    vector<shared_ptr<State>> currentBeam;
    auto startState = make_shared<State>(initialState);
    currentBeam.push_back(startState);
    result.nodesGenerated++;

    int nodesExpanded = 0;
    unordered_set<State, StateHasher> visited;

    while (!currentBeam.empty()) {
        vector<shared_ptr<State>> nextCandidates;

        for (auto& current : currentBeam) {
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

            nodesExpanded++;
            for (auto& nextS : problem.getSuccessors(*current)) {
                auto next = make_shared<State>(nextS);
                next->parent = current;
                if (visited.find(*next) == visited.end()) {
                    nextCandidates.push_back(next);
                    result.nodesGenerated++;
                }
            }
        }

        sort(nextCandidates.begin(), nextCandidates.end(), [&](const shared_ptr<State>& a, const shared_ptr<State>& b) {
            return h(*a, problem) < h(*b, problem);
        });

        currentBeam.clear();
        for (int i = 0; i < min((int)nextCandidates.size(), beamWidth); ++i) {
            currentBeam.push_back(nextCandidates[i]);
            visited.insert(*nextCandidates[i]);
        }
    }

    auto endT = chrono::steady_clock::now();
    result.executionTimeMs = chrono::duration_cast<chrono::milliseconds>(endT - startT).count();
    result.nodesExpanded = nodesExpanded;
    return result;
}
