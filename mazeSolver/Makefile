# Resolve the absolute path of the Git repository root
GIT_ROOT := $(shell git rev-parse --show-toplevel)

# Variable Definitions
CC        := gcc
CFLAGS    := -Wall -Wextra -g -O0
LDFLAGS   := 
TARGET    := mazeSolver
SRC       := mazeSolver.c
BUILD_DIR := $(GIT_ROOT)/build/local

# Execution path
EXEC      := $(BUILD_DIR)/$(TARGET)

.PHONY: all clean run debug

all: $(EXEC)

$(EXEC): $(SRC)
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) $(SRC) -o $(EXEC) $(LDFLAGS)

run: $(EXEC)
	@$(EXEC)

debug: $(EXEC)
	gdb $(EXEC)

clean:
	rm -rf $(GIT_ROOT)/build/local

