CC = gcc
CFLAGS = -Wall -Wextra -pedantic -std=c11 -fPIC
INCLUDE_DIR = include
SRC_DIR = src
EXAMPLES_DIR = examples
BUILD_DIR = build

LIB_SRC = $(SRC_DIR)/fswatcher.c
LIB_OBJ = $(BUILD_DIR)/fswatcher.o
STATIC_LIB = $(BUILD_DIR)/libfswatcher.a
DYNAMIC_LIB = $(BUILD_DIR)/libfswatcher.so
EXAMPLE_SRC = $(EXAMPLES_DIR)/example.c
EXAMPLE_OBJ = $(BUILD_DIR)/example.o
EXAMPLE_STATIC = $(BUILD_DIR)/example_static
EXAMPLE_DYNAMIC = $(BUILD_DIR)/example_dynamic

all: $(STATIC_LIB) $(DYNAMIC_LIB) $(EXAMPLE_STATIC) $(EXAMPLE_DYNAMIC)

$(STATIC_LIB): $(LIB_OBJ)
	ar rcs $@ $^

$(DYNAMIC_LIB): $(LIB_OBJ)
	$(CC) -shared -o $@ $^

$(EXAMPLE_STATIC): $(EXAMPLE_OBJ) $(STATIC_LIB)
	$(CC) $(CFLAGS) -o $@ $^ -lpthread

$(EXAMPLE_DYNAMIC): $(EXAMPLE_OBJ) $(DYNAMIC_LIB)
	$(CC) $(CFLAGS) -o $@ $^ -L$(BUILD_DIR) -lfswatcher -lpthread

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -I$(INCLUDE_DIR) -c $< -o $@

$(BUILD_DIR)/%.o: $(EXAMPLES_DIR)/%.c
	$(CC) $(CFLAGS) -I$(INCLUDE_DIR) -c $< -o $@

clean:
	rm -f $(BUILD_DIR)/*.o $(STATIC_LIB) $(DYNAMIC_LIB) $(EXAMPLE_STATIC) $(EXAMPLE_DYNAMIC)

.PHONY: all clean
