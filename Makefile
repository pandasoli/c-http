INC_DIR = include
SRC_DIR = src
OBJ_DIR = obj
LIB_DIR = lib
BIN_DIR = bin
TEST_DIR = tests

LIB_NAME = chttp
LIB = $(LIB_DIR)/lib$(LIB_NAME)
SRCS := $(wildcard $(SRC_DIR)/**/*.c $(SRC_DIR)/*.c)
OBJS := $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRCS))
TESTS := $(wildcard $(TEST_DIR)/*.c)

CC = gcc
CFLAGS = -g -O2 -Wall
LDFLAGS = -L$(LIB_DIR) -l$(LIB_NAME)

# Targets

.PHONY: all lib tests clean

all: clean lib tests

tests: lib $(TESTS:$(TEST_DIR)/%.c=$(BIN_DIR)/%)
lib: $(LIB)

clean:
	rm -rf $(OBJ_DIR) $(LIB_DIR) $(BIN_DIR)

# Files

$(LIB): $(OBJS)
	mkdir -p $(@D)
	ar rcs $@.a $^
	# $(CC) $(CFLAGS) -shared -o $@.so $^

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	mkdir -p $(@D)
	$(CC) $(CFLAGS) -c -o $@ $< -I$(INC_DIR)

$(BIN_DIR)/%: $(TEST_DIR)/%.c | $(LIB)
	mkdir -p $(@D)
	$(CC) $(CFLAGS) -o $@ $< -I$(INC_DIR) $(LDFLAGS)
