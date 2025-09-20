BUILD_DIR := build
SRC := src/main.c src/shell.c
SRC_TEST := tests/shell.c src/shell.c
CC := gcc
CFLAGS := -Wall -Iinclude
SHELL_EXE := $(BUILD_DIR)/shell
SHELL_EXE_TEST := $(BUILD_DIR)/test_shell

all: clean shell

shell:
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) $(SRC) -o $(SHELL_EXE) -lreadline

test: clean
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) $(SRC_TEST) -o $(SHELL_EXE_TEST) -lreadline

clean:
	rm -rf $(BUILD_DIR)

run_shell:
	./$(SHELL_EXE)

run_test:
	./$(SHELL_EXE_TEST)

install_dependencies:
	sudo apt update
	sudo apt install -y build-essential make 
	sudo apt-get install libreadline-dev
