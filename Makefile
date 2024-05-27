# Compilier options
CC := clang++
CFLAGS := -std=c++20

BUILD_DIR := build
SRC_DIR := src
OUTPUT := $(BUILD_DIR)/Graphics

# Libraries and include paths
INCLUDE_FLAGS := -Iinclude
LIB_FLAGS := -Llibraries -lGLEW -lglfw3

ifeq ($(shell uname), Darwin)
	LIB_FLAGS += -framework Cocoa -framework OpenGL -framework IOKit -framework CoreVideo -framework Metal
# else
# 	LIB_FLAGS += -lm -lpthread -ldl -lrt -lX11
endif

# Source files
SRC := $(wildcard $(SRC_DIR)/*.cpp)

# Obj files
OBJ := $(patsubst $(SRC_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(SRC))

all: $(OUTPUT) run

$(OUTPUT): $(OBJ)
	$(CC) $(OBJ) -o $(OUTPUT) $(LIB_FLAGS)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CC) -c $(CFLAGS) $(INCLUDE_FLAGS) $< -o $@

clean:
	rm -rf $(BUILD_DIR)/*

run: $(OUTPUT)
	./$(OUTPUT)

.PHONY: all clean run