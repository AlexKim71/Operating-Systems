#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main() {
    pid_t pid;

    pid = fork();

    if (pid == -1) {
        perror("Помилка fork");
        exit(EXIT_FAILURE);
    } else if (pid == 0) {
        // Child-процес
        printf("I am Zombie-process of gavrylov (PID: %d)\n", getpid());
        exit(EXIT_SUCCESS); // Child-процес завершується без виклику wait у parent
    } else {
        // Parent-процес
        sleep(30); // Parent-процес спить довше, ніж живе child
        printf("Parent-процес (PID: %d) завершує роботу.\n", getpid());
        exit(EXIT_SUCCESS);
    }

    return 0;
}