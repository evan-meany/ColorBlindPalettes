# Makefile

# Compiler options
CXX = g++
# CXXFLAGS = -std=c++17 -Wall -Wextra -pedantic -O2
CXXFLAGS = -std=c++17
CXXFLAGS += -w

# Directories
SRC_DIR = src
INCLUDE_DIR = include
BUILD_DIR = build

# Source files and object files
SRC_FILES = $(wildcard $(SRC_DIR)/*.cpp)
OBJ_FILES = $(patsubst $(SRC_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(SRC_FILES))

# Executable name
EXECUTABLE = $(BUILD_DIR)/test

# Default target
all: $(EXECUTABLE)

# Link the object files to create the executable
$(EXECUTABLE): $(OBJ_FILES)
	$(CXX) $(CXXFLAGS) $^ -o $@

# Compile the source files into object files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -I $(INCLUDE_DIR) -c $< -o $@

# Create the build directory if it does not exist
$(BUILD_DIR):
	mkdir $(BUILD_DIR)

# Clean up the object files and executable
clean:
	rm -f $(OBJ_FILES) $(EXECUTABLE)

run: $(EXECUTABLE)
	./$(EXECUTABLE)
