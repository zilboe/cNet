PROJECT = cNet
PROJECT_GDB = cNet_gdb
CC = gcc
INCLUDE_DIR = ./include
CFLAGS = -Wall -I$(INCLUDE_DIR)
OBJ_DIR = build
TARGET = $(OBJ_DIR)/$(PROJECT)
SRC_DIR = src
TESTS_DIR = tests
GDB_TARGET = $(OBJ_DIR)/$(PROJECT_GDB)
SRCS := $(wildcard $(SRC_DIR)/*.c) $(wildcard $(SRC_DIR)/*/*.c) $(wildcard $(SRC_DIR)/*/*/*.c)
OBJS := $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRCS))

all: $(TARGET)
$(TARGET): $(OBJS)
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -o $@ $^
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c -o $@ $<

gdb: $(GDB_TARGET)
$(GDB_TARGET): $(OBJS)
	@mkdir -p $(OBJ_DIR)
	$(CC) -g $(CFLAGS) -o $@ $^
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) -g $(CFLAGS) -c -o $@ $<

clean:
	rm -rf $(OBJ_DIR)/*

test: all
	@echo "Running tests..."
	@for t in $(wildcard $(TESTS_DIR)/*.c); do \
		echo "Testing $$t..."; \
		$(CC) -DTEST $(CFLAGS) -o $(OBJ_DIR)/$${t##*/} $$t $(TARGET); \
		$(OBJ_DIR)/$${t##*/}; \
	done

install:
	cp $(TARGET) .
	
install_gdb:
	cp $(GDB_TARGET) .