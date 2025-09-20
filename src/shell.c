#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include "shell.h"
#include "colors.h"
#include <dirent.h>
#include <sys/stat.h>
#include <errno.h>
#include <readline/readline.h>
#include <readline/history.h>

// Built-in ls
void _ls() {
    DIR *d = opendir(".");
        if (!d) { _perror("opendir"); return; }

    struct dirent *entry;
    struct stat st;

    while ((entry = readdir(d)) != NULL) {
        // Saltar ocultos (los que empiezan con '.')
        if (entry->d_name[0] == '.') continue;

        if (stat(entry->d_name, &st) == -1) continue;

        if (S_ISDIR(st.st_mode)) {
            printf("\033[32m%s\033[0m  ", entry->d_name); // verde para carpetas
        } else {
            printf("%s  ", entry->d_name);
        }
    }
    printf("\n");
    closedir(d);
}

// Built-in perror con color
void _perror(const char *context) {
    fprintf(stderr, RED);
    perror(context);
    fprintf(stderr, RESET);
}

// Separa los comandos por pipes
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

// Separa el commando por espacios
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

// Ejecuta los pipes
void execute_piped(char *line) {
    // Variables
    char *commands[64];
    int num_cmds = split_pipes(line, commands); // Dividir por pipes
    int pipefds[2*(num_cmds-1)];                // File descriptors para los pipes

    // Crear pipes
    for (int i = 0; i < num_cmds-1; i++) {
        if (pipe(pipefds + i*2) < 0) { 
            _perror("pipe"); 
            exit(1); 
        }
    }

    // Crear procesos para cada comando
    for (int i = 0; i < num_cmds; i++) {
        int pid = fork();
        if (pid == 0) {
            if (i > 0) dup2(pipefds[(i-1)*2], STDIN_FILENO);
            if (i < num_cmds-1) dup2(pipefds[i*2+1], STDOUT_FILENO);

            for (int j = 0; j < 2*(num_cmds-1); j++) close(pipefds[j]);

            char *args[MAX_ARGS];
            parse_command(commands[i], args);
            if (execvp(args[0], args) == -1) {
                if (errno == ENOENT) {
                    fprintf(stderr, RED "command not found: %s\n" RESET, args[0]);
                } else {
                    _perror("execvp");
                }
                exit(1);
            }
        }
    }

    for (int i = 0; i < 2*(num_cmds-1); i++) close(pipefds[i]);
    for (int i = 0; i < num_cmds; i++) wait(NULL);
}

// Procesos pre-execute_piped
void execute_command(char *line) {
    // Manejo de línea vacía
    if (line == NULL || *line == '\0') return;

    // Exit
    if (strcmp(line, "exit") == 0 || strcmp(line, "EXIT") == 0 || strcmp(line, "Exit") == 0) exit(0);

    // Manejo de cd ~ y cd <ruta>
    if (strncmp(line, "cd", 2) == 0) {
        char *path = line + 2;
        while (*path == ' ') path++;

        if (strcmp(path, "~") == 0) {
            path = getenv("HOME");  // reemplaza ~ por la ruta home
        }

        if (chdir(path) != 0) _perror("cd");
        return;
    }

    // Built-in ls
    if (strcmp(line, "ls") == 0) {
        _ls();
        return;
    }

    // Ejecutar comando (con o sin pipes)
    execute_piped(line);
}

// Función principal del shell, cada comando inicia aquí
void command() {
    // Variables
    char prompt[MAX_PATH + 30]; // Espacio extra para colores y texto fijo
    char cwd[MAX_PATH];
    char *line = NULL;

    // Obtener directorio actual
    if (getcwd(cwd, sizeof(cwd)) == NULL) {
        perror("getcwd");
        strcpy(cwd, "?"); // en caso de error
    }

    // Reemplazo de /home/usuario por ~ y obtener prompt
    char *inicio = getenv("HOME");
    if (inicio) {
        char *reemplazo = MAGENTA"~"GREEN;
        size_t len_inicio = strlen(inicio);
        size_t len_reemplazo = strlen(reemplazo);

        if (strncmp(cwd, inicio, len_inicio) == 0) {
            memmove(cwd + len_reemplazo, cwd + len_inicio, strlen(cwd) - len_inicio + 1);
            memcpy(cwd, reemplazo, len_reemplazo);
        }
    }
    snprintf(prompt, sizeof(prompt), "%sBCRZ:%s%s%s> ", CYAN, GREEN, cwd, RESET); // Dejar prompt listo.

    // Leer línea con readline
    line = readline(prompt);
    if (line && *line) add_history(line); // Agregar a historial si no está vacía

    // Ejecutar comando 
    execute_command(line);

    free(line);
}
