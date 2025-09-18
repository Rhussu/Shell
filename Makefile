BUILD_DIR := build
SRC := src/main.c src/shell.c
SRC_TEST := tests/shell.c src/shell.c
CC := gcc
CFLAGS := -Wall -Iinclude
SHELL_EXE := $(BUILD_DIR)/shell
SHELL_EXE_TEST := $(BUILD_DIR)/test_shell

all: shell

shell: $(SRC)
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) $(SRC) -o $(SHELL_EXE)

test: $(SRC_test)
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) $(SRC_TEST) -o $(SHELL_EXE_TEST)

clean:
	rm -rf $(BUILD_DIR)

deps:
	sudo apt update
	sudo apt install -y build-essential make libreadline-dev
