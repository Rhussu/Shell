#ifndef SHELL_H
#define SHELL_H

#define MAX_INPUT 1024
#define MAX_ARGS 100
#define MAX_CMD 1024
#define MAX_PATH 1024


int split_pipes(char* line, char** commands);
void execute_piped(char* line);
int read_command();
int parse_command(char* line, char** args);

#endif
