#include <unistd.h>
#include <sys/wait.h>
#include "shell.h"

int main() {
    int error = -1;
    while (error == -1) {
        error = read_command();
    }
    return error;
}
