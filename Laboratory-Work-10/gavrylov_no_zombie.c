#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main() {
    pid_t pid;
    int status; // Змінна для зберігання статусу завершення child-процесу

    pid = fork();

    if (pid == -1) {
        perror("Помилка fork");
        exit(EXIT_FAILURE);
    } else if (pid == 0) {
        // Child-процес
        printf("Child-процес (PID: %d) завершує роботу.\n", getpid());
        exit(EXIT_SUCCESS);
    } else {
        // Parent-процес
        sleep(10); // Parent-процес спить деякий час
        printf("Parent-процес (PID: %d) очікує завершення child-процесу...\n", getpid());
        wait(&status); // Очікування завершення child-процесу

        if (WIFEXITED(status)) {
            printf("Child-процес завершився нормально з кодом виходу: %d\n", WEXITSTATUS(status));
        } else if (WIFSIGNALED(status)) {
            printf("Child-процес завершився через сигнал: %d\n", WTERMSIG(status));
        }

        printf("Parent-процес (PID: %d) завершує роботу.\n", getpid());
        exit(EXIT_SUCCESS);
    }

    return 0;
}