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

#include <sys/resource.h>
#include <sys/time.h>
#include <signal.h>

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
int execute_piped(char *line) {
    char *commands[64];
    int num_cmds = split_pipes(line, commands);
    int pipefds[2*(num_cmds-1)];
    pid_t pids[64];

    for (int i = 0; i < num_cmds-1; i++) {
        if (pipe(pipefds + i*2) < 0) {
            _perror("pipe");
            exit(1);
        }
    }

    for (int i = 0; i < num_cmds; i++) {
        int pid = fork();
        if (pid == 0) {
            if (i > 0) dup2(pipefds[(i-1)*2], STDIN_FILENO);
            if (i < num_cmds-1) dup2(pipefds[i*2+1], STDOUT_FILENO);

            for (int j = 0; j < 2*(num_cmds-1); j++) close(pipefds[j]);

            char *args[MAX_ARGS];
            parse_command(commands[i], args);
            execvp(args[0], args);

            // Si falla execvp
            if (errno == ENOENT) {
                fprintf(stderr, RED "command not found: %s\n" RESET, args[0]);
            } else {
                _perror("execvp");
            }
            _exit(127); // marcar error
        }
        pids[i] = pid;
    }

    for (int i = 0; i < 2*(num_cmds-1); i++) close(pipefds[i]);

    int failed = 0;
    for (int i = 0; i < num_cmds; i++) {
        int status;
        waitpid(pids[i], &status, 0);
        if (!WIFEXITED(status) || WEXITSTATUS(status) != 0) failed = 1;
    }

    return failed;
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
        
        if ((strcmp(path, "~") == 0) || (strcmp(path, "") == 0)) {
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

        // Comando miprof
    if (strncmp(line, "miprof", 6) == 0) {
        run_miprof(line);
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
//Parte 2


// Ejecuta miprof completo
void run_miprof(char *line) {
    char *args[MAX_ARGS];
    char *mode = strtok(line, " "); // "miprof"
    mode = strtok(NULL, " ");       // "ejec | ejecsave | ejecutar"

    if (!mode) {
        fprintf(stderr, "Uso: miprof [ejec|ejecsave archivo|ejecutar maxtiempo] comando args...\n");
        return;
    }

    char *savefile = NULL;
    int timeout = -1;

    if (strcmp(mode, "ejecsave") == 0) {
        savefile = strtok(NULL, " ");
        if (!savefile) {
            fprintf(stderr, "Debe indicar un archivo para ejecsave.\n");
            return;
        }
    } else if (strcmp(mode, "ejecutar") == 0) {
        char *t = strtok(NULL, " ");
        if (!t) {
            fprintf(stderr, "Debe indicar un tiempo máximo en segundos.\n");
            return;
        }
        timeout = atoi(t);
    }

    char *cmdline = strtok(NULL, "");
    if (!cmdline) {
        fprintf(stderr, "Debe indicar un comando a ejecutar.\n");
        return;
    }

    struct timeval start, end;
    gettimeofday(&start, NULL);

    pid_t pid = fork();
    if (pid < 0) { perror("fork"); return; }

    if (pid == 0) { // hijo
        setpgid(0,0); // grupo propio

        // Redirección si es ejecsave
        if (savefile) {
            FILE *devnull = fopen("/dev/null","w");
            if (devnull) {
                dup2(fileno(devnull), STDOUT_FILENO);
                dup2(fileno(devnull), STDERR_FILENO);
                fclose(devnull);
            }
        }

        int ret;
        if (strchr(cmdline,'|')) ret = execute_piped(cmdline);
        else {
            parse_command(cmdline, args);
            execvp(args[0], args);
            if (errno == ENOENT) _exit(127); // comando no encontrado
            else _exit(126); // otro error exec
        }
        _exit(ret ? 127 : 0);
    } 
    else { // padre
        int status;
        struct rusage usage;
        int timed_out = 0;

        if (timeout > 0) {
            int elapsed = 0;
            while (elapsed < timeout) {
                pid_t w = waitpid(pid, &status, WNOHANG);
                if (w == pid) break;
                sleep(1);
                elapsed++;
            }
            if (elapsed >= timeout) {
                timed_out = 1;
                kill(-pid, SIGKILL);
                waitpid(pid, &status, 0);
            }
        } else {
            waitpid(pid, &status, 0);
        }

        gettimeofday(&end,NULL);
        getrusage(RUSAGE_CHILDREN,&usage);

        double real = (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec)/1e6;
        double user = usage.ru_utime.tv_sec + usage.ru_utime.tv_usec/1e6;
        double sys  = usage.ru_stime.tv_sec + usage.ru_stime.tv_usec/1e6;
        long maxrss = usage.ru_maxrss;

        // Detectar si comando no existe
        int command_not_found = WIFEXITED(status) && WEXITSTATUS(status) == 127;

        if (command_not_found) {
            fprintf(stderr, RED "Error: comando no encontrado. No se muestran resultados miprof.\n" RESET);
            return;
        }

        if (timed_out) {
            // comando existía, pero se excedió el tiempo
            printf("\n--- Resultados miprof ---\n");
            printf("Tiempo real:   %.6f s\n", real);
            printf("Tiempo usuario: %.6f s\n", user);
            printf("Tiempo sistema: %.6f s\n", sys);
            printf("Memoria pico:   %ld KB\n", maxrss);
            fprintf(stderr, "Tiempo excedido (%d s). Matando proceso.\n", timeout);
        }
        else if (!WIFEXITED(status) || WEXITSTATUS(status)!=0) {
            // otro error (comando existe pero falló)
            fprintf(stderr, RED "Error: comando falló. No se muestran resultados miprof.\n" RESET);
        }
        else {
            // éxito
            if (savefile) {
                FILE *f = fopen(savefile,"a");
                if(f){
                    fprintf(f, "\nComando: %s\n", cmdline);
                    fprintf(f, "Tiempo real:   %.6f s\n", real);
                    fprintf(f, "Tiempo usuario: %.6f s\n", user);
                    fprintf(f, "Tiempo sistema: %.6f s\n", sys);
                    fprintf(f, "Memoria pico:   %ld KB\n", maxrss);
                    fclose(f);
                    printf("Resultados guardados en %s\n", savefile);
                }
            } else {
                printf("\n--- Resultados miprof ---\n");
                printf("Tiempo real:   %.6f s\n", real);
                printf("Tiempo usuario: %.6f s\n", user);
                printf("Tiempo sistema: %.6f s\n", sys);
                printf("Memoria pico:   %ld KB\n", maxrss);
            }
        }
    }
}
