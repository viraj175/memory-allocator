CC = gcc
CFLAGS = -Wall -Wextra -O2 -g
SRC_DIR = src
BUILD_DIR = build

# Create build directory
$(shell mkdir -p $(BUILD_DIR))

all: $(BUILD_DIR)/test_allocator

$(BUILD_DIR)/test_allocator: $(SRC_DIR)/malloc.c $(SRC_DIR)/main.c
	$(CC) $(CFLAGS) -o $@ $^

run: all
	./$(BUILD_DIR)/test_allocator

clean:
	rm -rf $(BUILD_DIR)

.PHONY: all run clean
