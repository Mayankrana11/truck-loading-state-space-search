#pragma once

#include <vector>
#include <string>
#include <memory>
#include "State.h"

namespace Utils {
    std::vector<std::string> reconstructPath(std::shared_ptr<State> state);
}
