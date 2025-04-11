# Compiler settings
CXX = g++
CXXFLAGS = -std=c++17 -Wall -O2

# Directories
SRC_DIR = src
BIN_DIR = bin

# Source files and target
SRCS = $(wildcard $(SRC_DIR)/*.cpp)
TARGET = $(BIN_DIR)/main

# Platform-specific settings
ifeq ($(OS),Windows_NT)
	TARGET := $(TARGET).exe
	MKDIR = if not exist $(BIN_DIR) mkdir $(BIN_DIR)
	RM = del /Q /F
	RM_DIR = rmdir /Q /S
else
	MKDIR = mkdir -p $(BIN_DIR)
	RM = rm -f
	RM_DIR = rm -rf
endif

# Default target
all: $(TARGET)

# Create bin directory if it doesn't exist
$(BIN_DIR):
	$(MKDIR)

# Compile
$(TARGET): $(SRCS) | $(BIN_DIR)
	$(CXX) $(CXXFLAGS) -o $@ $^

# Clean
clean:
	$(RM) $(TARGET)

# Deep clean (remove bin directory)
distclean: clean
	$(RM_DIR) $(BIN_DIR)

.PHONY: all clean distclean