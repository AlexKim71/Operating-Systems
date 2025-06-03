// pipe_write.c
// Програма для запису в неіменований канал та запуску pipe_read як дочірнього процесу.

#include <stdio.h>    // Для printf, fprintf, perror
#include <stdlib.h>   // Для EXIT_SUCCESS, EXIT_FAILURE, exit
#include <unistd.h>   // Для pipe, fork, close, write, getpid, execlp
#include <string.h>   // Для strlen, strcpy
#include <sys/wait.h> // Для waitpid

#define BUFFER_SIZE 256
// Повідомлення для передачі в канал. Використовуємо ваше прізвище.
#define MESSAGE "The Laboratory Work of Vyacheslavovich"

int main()
{
    int pipefd[2];                     // Масив для зберігання дескрипторів каналу:
                                       // pipefd[0] - для читання, pipefd[1] - для запису
    pid_t pid;                         // Для зберігання PID дочірнього процесу
    char read_fd_str[10];              // Буфер для строкового представлення дескриптора читання
    char *surname = "Vyacheslavovich"; // Ваше прізвище для виводу

    // ---- Крок 1: Створює неіменований канал ----
    if (pipe(pipefd) == -1)
    {
        perror("Помилка створення каналу (pipe)");
        return EXIT_FAILURE;
    }

    // ---- Крок 2: Створює child-процес ----
    pid = fork();

    if (pid == -1)
    {
        perror("Помилка створення дочірнього процесу (fork)");
        // Закриваємо дескриптори каналу, створені раніше
        close(pipefd[0]);
        close(pipefd[1]);
        return EXIT_FAILURE;
    }

    if (pid == 0)
    {
        // ---- КОД ДОЧІРНЬОГО ПРОЦЕСУ ----
        // 2.1: Закриваємо записуючий кінець каналу в дочірньому процесі,
        //      оскільки він буде тільки читати.
        close(pipefd[1]);

        // 2.2: Перетворюємо числовий дескриптор для читання на рядок,
        //      щоб передати його як аргумент командного рядка для pipe_read.
        sprintf(read_fd_str, "%d", pipefd[0]);

        // 2.3: Завантажуємо новий програмний образ pipe_read з викликом pipe_read
        //      та її аргументом (дескриптором для читання).
        // execlp шукає програму в PATH, але краще вказати явний шлях "./"
        execlp("./pipe_read", "pipe_read", read_fd_str, NULL);

        // Якщо execlp повернув управління сюди, значить сталася помилка
        perror("Помилка запуску програми pipe_read (execlp)");
        // Важливо вийти з дочірнього процесу у разі помилки execlp
        exit(EXIT_FAILURE);
    }
    else
    {
        // ---- КОД БАТЬКІВСЬКОГО ПРОЦЕСУ ----
        // 2.4: Закриваємо читаючий кінець каналу в батьківському процесі,
        //      оскільки він буде тільки писати.
        close(pipefd[0]);

        // ---- Крок 3: Parent-процес передає у канал повідомлення ----
        // Записуємо повідомлення в канал
        if (write(pipefd[1], MESSAGE, strlen(MESSAGE)) == -1)
        {
            perror("Помилка запису в канал");
            close(pipefd[1]); // Закриваємо навіть у разі помилки
            return EXIT_FAILURE;
        }

        // Важливо: Закриваємо записуючий кінець каналу після запису.
        // Це дозволяє дочірньому процесу отримати "кінець файлу" (EOF),
        // коли він прочитає всі дані. Без цього read() в дочірньому
        // процесі може зависнути, очікуючи більше даних.
        close(pipefd[1]);

        // ---- Крок 4: Parent-процес виводить на екран повідомлення про себе ----
        printf("I'm parent process of %s with pid=%d\n", surname, getpid());

        // ---- Крок 5: Parent-процес виводить розмір переданого повідомлення ----
        printf("Розмір повідомлення, переданого у канал: %zu байтів.\n", strlen(MESSAGE));

        // Чекаємо завершення дочірнього процесу
        int status;
        if (waitpid(pid, &status, 0) == -1)
        {
            perror("Помилка очікування дочірнього процесу (waitpid)");
            return EXIT_FAILURE;
        }

        printf("Дочірній процес завершив роботу.\n");
    }

    return EXIT_SUCCESS; // Успішне завершення програми
}