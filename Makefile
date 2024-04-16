CC = gcc
CFLAGS = -Wall -g -Iinclude
LDFLAGS =
TARGET_DIR = bin
OBJ_DIR = obj
SRC_DIR = src

# List of source files
SRCS := $(wildcard $(SRC_DIR)/*.c)

# List of object files
OBJS := $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SRCS))

# Default target
all: folders servidor cliente

# Create necessary folders
folders:
	@mkdir -p $(OBJ_DIR) $(TARGET_DIR)

# Build servidor and cliente binaries
servidor: $(TARGET_DIR)/servidor

cliente: $(TARGET_DIR)/cliente

# Link servidor binary
$(TARGET_DIR)/servidor: $(OBJ_DIR)/servidor.o
	$(CC) $(LDFLAGS) $^ -o $@

# Link cliente binary
$(TARGET_DIR)/cliente: $(OBJ_DIR)/cliente.o
	$(CC) $(LDFLAGS) $^ -o $@

# Compile source files to object files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# Clean build files
clean:
	rm -f $(OBJ_DIR)/* $(TARGET_DIR)/*

# Phony target to prevent conflicts with file names
.PHONY: all clean folders servidor cliente
