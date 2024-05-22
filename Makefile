# Directories
S_DIR = src
B_DIR = build
INCLUDE_DIRS = -I/opt/homebrew/include

# Files
S_FILES = $(wildcard $(S_DIR)/*.cpp)
OBJ_FILES = $(patsubst $(S_DIR)/%.cpp,$(B_DIR)/%.o,$(S_FILES))

# Libraries for GLEW and GLFW
GLEW_LIB = -lGLEW
GLFW_LIB = -lglfw -framework Cocoa -framework OpenGL -framework IOKit -framework CoreVideo

# Output
EXEC = $(B_DIR)/Graphics

all: $(EXEC)

$(EXEC): $(OBJ_FILES)
	clang++ $^ -o $@ -std=c++20 -L/opt/homebrew/lib $(GLEW_LIB) $(GLFW_LIB)

$(B_DIR)/%.o: $(S_DIR)/%.cpp
	clang++ -c $< -o $@ $(INCLUDE_DIRS) -std=c++20

run: $(EXEC)
	./$(EXEC)

clean:
	rm -rf $(B_DIR)/*
