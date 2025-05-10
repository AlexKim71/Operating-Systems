#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>

int main() {
    pid_t pid;
    int n = 5; // Номер варіанту

    pid = fork();

    if (pid == -1) {
        perror("Помилка fork");
        exit(EXIT_FAILURE);
    } else if (pid > 0) {
        // Parent-процес
        printf("Parent-процес (PID: %d) розпочав роботу.\n", getpid());
        sleep(n + 1); // Очікування n+1 = 6 секунд
        printf("Parent-процес (PID: %d) завершує роботу.\n", getpid());
        exit(EXIT_SUCCESS);
    } else {
        // Child-процес
        printf("Child-процес (PID: %d, PPID: %d) розпочав роботу.\n", getpid(), getppid());
        for (int i = 0; i < 2 * n + 1; ++i) { // Цикл 2*n+1 = 11 разів
            sleep(1); // Затримка в 1 секунду
            printf("Parent of gavrylov (PID: %d, PPID: %d)\n", getpid(), getppid());
        }
        printf("Child-процес (PID: %d) завершує роботу.\n", getpid());
        exit(EXIT_SUCCESS);
    }

    return 0;
}