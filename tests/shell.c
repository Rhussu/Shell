#include "../include/shell.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>


int test_cdhome() {
    char* command = "cd ~";
    execute_command(command);
    char cwd[MAX_PATH];
    if (getcwd(cwd, sizeof(cwd)) == NULL) {
        perror("getcwd");
        return 1;
    }
    char *home = getenv("HOME");
    if (!home) {
        fprintf(stderr, "No se pudo obtener la variable HOME\n");
        return 1;
    }
    if (strcmp(cwd, home) != 0) {
        printf("Fallo: %s no lleva a HOME. cwd=%s, HOME=%s\n", command, cwd, home);
        return 1;
    }
    return 0;
}

int main() {
    int errors = 0;

    // Ejecutar tests
    printf("Ejecutando test de \"cd ~\"...\n");
    errors += test_cdhome();

    if (errors == 0)
        printf("Todos los tests pasaron ✅\n");
    else
        printf("Algunos tests fallaron ❌\n");

    return errors;
}
