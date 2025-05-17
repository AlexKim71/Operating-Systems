#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

// Глобальна змінна для позначення отримання сигналу
volatile sig_atomic_t signal_received = 0;

// Обробник сигналу SIGUSR2
void sigusr2_handler(int signum) {
    if (signum == SIGUSR2) {
        signal_received = 1;
    }
}

int main() {
    char gavrylov[] = "gavrylov";

    // Реєстрація обробника сигналу SIGUSR2
    if (signal(SIGUSR2, sigusr2_handler) == SIG_ERR) {
        perror("Помилка при встановленні обробника сигналу SIGUSR2");
        exit(EXIT_FAILURE);
    }

    printf("Процес очікує на сигнал SIGUSR2 (PID: %d)...\n", getpid());

    // Очікування сигналу
    while (!signal_received) {
        pause(); // Призупиняє процес до отримання сигналу
    }

    // Після отримання сигналу виводимо повідомлення у правильному форматі
    printf("Process of %s got signal\n", gavrylov);

    // Завершення процесу після обробки сигналу
    exit(EXIT_SUCCESS);

    return 0; // Цей рядок ніколи не буде досягнуто, але потрібен для компілятора
}