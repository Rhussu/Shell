
#ifndef SHELL_H
#define SHELL_H

#define MAX_INPUT 1024
#define MAX_ARGS 100
#define MAX_CMD 1024
#define MAX_PATH 1024

// Parsing
int split_pipes(char* line, char** commands);
int parse_command(char* line, char** args);

// Pre execution and execution
void execute_piped(char* line);
void execute_command(char* line);
void command();

// Built-ins
void _ls();
void _perror(const char *text);

// miprof
void run_miprof(char *line);
#endif
