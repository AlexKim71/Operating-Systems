#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main() {
    pid_t pid;
    int n = 5; // Номер варіанту

    pid = fork();

    if (pid == -1) {
        perror("Помилка fork");
        exit(EXIT_FAILURE);
    } else if (pid == 0) {
        // Child-процес
        printf("Child of gavrylov is finished (PID: %d)\n", getpid());
        exit(EXIT_SUCCESS); // Child-процес завершується
    } else {
        // Parent-процес
        sleep(3 * n); // Очікування 3 * 5 = 15 секунд
        printf("Parent-процес (PID: %d) завершує роботу, не чекаючи child.\n", getpid());
        exit(EXIT_SUCCESS);
    }

    return 0;
}