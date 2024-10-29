# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -g
LD_FLAGS = -Wl,--version-script=$(SRC_DIR)/export.map

# Directories
SRC_DIR = src
BUILD_DIR = build
LIB_DIR = $(shell pwd)/$(BUILD_DIR)/lib

VER_MAJ = 5
VER_MIN = 4.6

# Targets
SERVER_BIN = $(BUILD_DIR)/server
ISO_LIB = $(LIB_DIR)/libisolation.so.$(VER_MAJ).$(VER_MIN)
TEST_BIN = $(BUILD_DIR)/caged_prog

# Source files
SERVER_SRC = $(SRC_DIR)/server.c
ISO_SRC = $(SRC_DIR)/isolation.c
TEST_SRC = $(SRC_DIR)/caged_prog.c

# Build all targets
all: $(SERVER_BIN) $(ISO_LIB) $(TEST_BIN)

# Build server binary
$(SERVER_BIN): $(SERVER_SRC)
	mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -o $(SERVER_BIN) $(SERVER_SRC) -llzma

# Build client as a shared object (.so)
$(ISO_LIB): $(ISO_SRC)
	mkdir -p $(LIB_DIR)
	$(CC) $(CFLAGS) -fPIC -shared $(LD_FLAGS) -o $(ISO_LIB) $(ISO_SRC)
	ln -s $(ISO_LIB) $(LIB_DIR)/libisolation.so.$(VER_MAJ)
	ln -s $(LIB_DIR)/libisolation.so.$(VER_MAJ) $(LIB_DIR)/libisolation.so

# Build test program linking to libisolation
$(TEST_BIN): $(TEST_SRC) $(ISO_LIB)
	mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -L$(LIB_DIR) -Wl,-rpath=$(LIB_DIR) -o $(TEST_BIN) $(TEST_SRC) -lisolation

# Clean build files
clean:
	rm -f $(SERVER_BIN) $(LIB_DIR)/* $(TEST_BIN) build/*.d

.PHONY: all clean
