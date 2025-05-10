#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Використання: %s <PID_отримувача>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    pid_t receiver_pid = atoi(argv[1]);

    if (receiver_pid <= 0) {
        fprintf(stderr, "Невірний PID: %s\n", argv[1]);
        exit(EXIT_FAILURE);
    }

    if (kill(receiver_pid, SIGUSR2) == -1) {
        perror("Помилка при надсиланні сигналу SIGUSR2");
        exit(EXIT_FAILURE);
    }

    printf("Сигнал SIGUSR2 успішно надіслано процесу з PID: %d\n", receiver_pid);

    return 0;
}