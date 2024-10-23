# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -g

# Directories
SRC_DIR = src
BUILD_DIR = build
LIB_DIR = $(BUILD_DIR)/lib

# Targets
SERVER_BIN = $(BUILD_DIR)/server
ISO_LIB = $(LIB_DIR)/libisolation.so
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
	$(CC) $(CFLAGS) -llzma -o $(SERVER_BIN) $(SERVER_SRC)

# Build client as a shared object (.so)
$(ISO_LIB): $(ISO_SRC)
	mkdir -p $(LIB_DIR)
	$(CC) $(CFLAGS) -fPIC -shared -o $(ISO_LIB) $(ISO_SRC)

# Build test program linking to libisolation
$(TEST_BIN): $(TEST_SRC) $(ISO_LIB)
	mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -L$(LIB_DIR) -Wl,-rpath=$(LIB_DIR) -lisolation -llzma -o $(TEST_BIN) $(TEST_SRC)

# Clean build files
clean:
	rm -f $(SERVER_BIN) $(ISO_LIB) $(TEST_BIN) build/*.d

.PHONY: all clean
