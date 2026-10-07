CXX = g++
CXXFLAGS = -std=c++17 -Iinclude -Wall -Wextra
SRC_DIR = src
OBJ_DIR = obj
BIN = TruckLoading.exe
OPT_BIN = test_optimal.exe
STATE_BIN = test_state.exe
PROBLEM_BIN = test_problem.exe
SEARCH_BIN = test_search.exe
HEURISTIC_BIN = test_heuristics.exe

# Find all .cpp files in src directory
SRCS = $(wildcard $(SRC_DIR)/*.cpp)
# Convert .cpp paths to .o paths in the obj directory
OBJS = $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(SRCS))

# We need a version of the source files without main.cpp for tests
CORE_OBJS = $(filter-out $(OBJ_DIR)/main.o, $(OBJS))

all: $(BIN)

$(BIN): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJ_DIR):
	@if not exist $(OBJ_DIR) mkdir $(OBJ_DIR)

optimal: $(CORE_OBJS)
	$(CXX) $(CXXFLAGS) -o $(OPT_BIN) $^ tests/test_optimality.cpp

state: $(CORE_OBJS)
	$(CXX) $(CXXFLAGS) -o $(STATE_BIN) $^ tests/test_state.cpp

problem: $(CORE_OBJS)
	$(CXX) $(CXXFLAGS) -o $(PROBLEM_BIN) $^ tests/test_problem.cpp

search: $(CORE_OBJS)
	$(CXX) $(CXXFLAGS) -o $(SEARCH_BIN) $^ tests/test_search.cpp

heuristic: $(CORE_OBJS)
	$(CXX) $(CXXFLAGS) -o $(HEURISTIC_BIN) $^ tests/test_heuristics.cpp

clean:
	@if exist $(OBJ_DIR) rmdir /s /q $(OBJ_DIR)
	@if exist $(BIN) del /q /f $(BIN)
	@if exist $(OPT_BIN) del /q /f $(OPT_BIN)
	@if exist $(STATE_BIN) del /q /f $(STATE_BIN)
	@if exist $(PROBLEM_BIN) del /q /f $(PROBLEM_BIN)
	@if exist $(SEARCH_BIN) del /q /f $(SEARCH_BIN)
	@if exist $(HEURISTIC_BIN) del /q /f $(HEURISTIC_BIN)

.PHONY: all clean optimal state problem search heuristic
