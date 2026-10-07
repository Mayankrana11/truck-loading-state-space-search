#include "Utils.h"
#include <algorithm>

namespace Utils {
    std::vector<std::string> reconstructPath(std::shared_ptr<State> state) {
        std::vector<std::string> path;
        while (state && state->action != "") {
            path.push_back(state->action);
            state = state->parent;
        }
        std::reverse(path.begin(), path.end());
        return path;
    }
}
