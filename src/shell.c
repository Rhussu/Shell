#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include "shell.h"
#include "colors.h"


int split_pipes(char *line, char **commands) {
    int count = 0;
    char *start = line;
    char *p = line;
    int in_quotes = 0;

    while (*p) {
        if (*p == '"') in_quotes = !in_quotes;
        else if (*p == '|' && !in_quotes) {
            *p = '\0';
            commands[count++] = start;
            start = p + 1;
        }
        p++;
    }
    commands[count++] = start;
    return count;
}

int parse_command(char *cmd, char **args) {
    int i = 0;
    char *p = cmd;

    while (*p) {
        while (*p == ' ' || *p == '\t') p++;
        if (*p == '\0') break;

        if (*p == '"') {
            p++;
            args[i] = p;
            while (*p && *p != '"') p++;
            if (*p) *p++ = '\0';
        } else {
            args[i] = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) *p++ = '\0';
        }
        i++;
    }
    args[i] = NULL;
    return i;
}

void execute_piped(char *line) {
    char *commands[64];
    int num_cmds = split_pipes(line, commands);
    int pipefds[2*(num_cmds-1)];

    for (int i = 0; i < num_cmds-1; i++) {
        if (pipe(pipefds + i*2) < 0) { perror("pipe"); exit(1); }
    }

    for (int i = 0; i < num_cmds; i++) {
        int pid = fork();
        if (pid == 0) {
            if (i > 0) dup2(pipefds[(i-1)*2], STDIN_FILENO);
            if (i < num_cmds-1) dup2(pipefds[i*2+1], STDOUT_FILENO);

            for (int j = 0; j < 2*(num_cmds-1); j++) close(pipefds[j]);

            char *args[MAX_ARGS];
            parse_command(commands[i], args);
            if (args[0] == NULL) exit(0); // comando vacío
            execvp(args[0], args);
            perror("execvp");
            exit(1);
        }
    }

    for (int i = 0; i < 2*(num_cmds-1); i++) close(pipefds[i]);
    for (int i = 0; i < num_cmds; i++) wait(NULL);
}


int read_command() {
    char input[MAX_CMD];
    char cwd[MAX_PATH];

    // Obtener directorio actual
    if (getcwd(cwd, sizeof(cwd)) == NULL) {
        perror("getcwd");
        strcpy(cwd, "?"); // fallback
    }

    char* inicio = "/home";
    char* reemplazo = MAGENTA"~"GREEN;
    char* str = cwd;
    size_t len_inicio = strlen(inicio);
    size_t len_reemplazo = strlen(reemplazo);
    if (strncmp(cwd, inicio, len_inicio) == 0) {
        memmove(str + len_reemplazo, str + len_inicio, strlen(str) - len_inicio + 1);
        memcpy(str, reemplazo, len_reemplazo);
    }
    // Mostrar prompt con path actual
    printf(CYAN "BCRZ:"GREEN"%s"RESET"> ", cwd);
    fflush(stdout);

    if (!fgets(input, MAX_CMD, stdin)) return -1;
    input[strcspn(input, "\n")] = 0;
    if (strcmp(input, "exit") == 0 || strcmp(input, "EXIT") == 0 || strcmp(input, "Exit") == 0) return 0;
    if (strncmp(input, "cd", 2) == 0) {
        char *path = input + 2;
        while (*path == ' ') path++;

        if (strcmp(path, "~") == 0) {
            path = getenv("HOME");  // reemplaza ~ por la ruta home
        }

        if (chdir(path) != 0) perror("cd");
        return -1;
    }

    if (strncmp(input, "cd", 2) == 0) {
        char *path = input + 2;
        while (*path == ' ') path++;
        if (chdir(path) != 0) perror("cd");
        return -1;
    }

    execute_piped(input);
    return -1;
}
