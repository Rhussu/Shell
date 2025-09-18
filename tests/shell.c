#include "../include/shell.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>


int main() {
    int errors = 1;

    // Ejecutar tests
    // errors += test();

    if (errors == 0)
        printf("Todos los tests pasaron ✅\n");
    else
        printf("Algunos tests fallaron ❌\n");

    return errors;
}
