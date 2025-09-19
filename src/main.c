#include <unistd.h>
#include <sys/wait.h>
#include "shell.h"

int main() {
    while (1) {
        command();
    }
    return 0;
}
