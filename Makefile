# Directories
BUILD_DIR := ./build
LIB_DIR := ./lib
INCLUDE_DIR := ./include
SRC_DIR := ./src
EXAMPLES_DIR := ./examples

# Compiler and flags
CC := gcc
CFLAGS := -I$(INCLUDE_DIR) -L$(LIB_DIR) -Wl,-rpath,$(LIB_DIR) -O2
LIBS := -lbf

# Targets
hp: $(BUILD_DIR)/hp_main

# Rules for building executables
$(BUILD_DIR)/hp_main: $(EXAMPLES_DIR)/hp_main.c $(SRC_DIR)/record.c $(SRC_DIR)/hp_file.c | $(BUILD_DIR)
	@echo "Compiling hp_main ..."
	$(CC) $(CFLAGS) $^ $(LIBS) -o $@

# Run target
run: hp
	@echo "Running..."
	rm -f ./data.db
	./build/hp_main

# Clean target
clean:
	@echo "Cleaning up..."
	rm -rf $(BUILD_DIR)
	rm -f ./data.db

# Ensure build directory exists
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)
