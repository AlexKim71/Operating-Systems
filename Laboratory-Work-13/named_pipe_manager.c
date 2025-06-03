// named_pipe_manager.c
// Програма, яка створює іменований канал, запускає процеси запису та читання,
// а потім видаляє канал.

#include <stdio.h>    // Для printf, fprintf, perror
#include <stdlib.h>   // Для EXIT_SUCCESS, EXIT_FAILURE, exit
#include <unistd.h>   // Для fork, close, read, write, getpid, execlp
#include <sys/stat.h> // Для mkfifo
#include <fcntl.h>    // Для open, O_RDONLY, O_WRONLY
#include <sys/wait.h> // Для waitpid
#include <string.h>   // Для strlen (не використовується для даних, лише для службових повідомлень)

#define FIFO_NAME "vyacheslavovich" // Назва іменованого каналу
#define PERMISSIONS 0660            // Права доступу: rw-rw----
#define BUFFER_SIZE 4096            // Розмір буфера для читання
#define SURNAME "Vyacheslavovich"   // Ваше прізвище для виводу

int main()
{
    pid_t pid_writer, pid_reader;
    int fifo_fd;
    char buffer[BUFFER_SIZE];
    ssize_t bytes_read;
    int status; // Для waitpid

    printf("--- Батьківський процес (PID: %d) починає роботу ---\n", getpid());

    // ---- Крок 2.3.1: Створити іменований канал з використанням команди mkfifo ----
    printf("1. Створення іменованого каналу '%s' з правами %o...\n", FIFO_NAME, PERMISSIONS);
    if (mkfifo(FIFO_NAME, PERMISSIONS) == -1)
    {
        perror("Помилка створення іменованого каналу (mkfifo)");
        // Якщо канал вже існує, це не критична помилка для подальших дій,
        // але для цього завдання це вважається помилкою створення.
        // Можна додати перевірку EEXIST, якщо потрібно ігнорувати.
        return EXIT_FAILURE;
    }
    printf("Канал '%s' успішно створено.\n", FIFO_NAME);

    // ---- Крок 2.3.2: Підключити до іменованого каналу процес, який буде в нього записувати ----
    // Створюємо дочірній процес для запису в FIFO
    pid_writer = fork();
    if (pid_writer == -1)
    {
        perror("Помилка fork для процесу запису");
        unlink(FIFO_NAME); // Видаляємо канал, якщо fork не вдався
        return EXIT_FAILURE;
    }

    if (pid_writer == 0)
    {
        // ---- КОД ДОЧІРНЬОГО ПРОЦЕСУ (ЗАПИСУЮЧИЙ) ----
        printf("\n--- Дочірній процес запису (PID: %d) починає роботу ---\n", getpid());
        printf("I'm writer process of %s with pid=%d\n", SURNAME, getpid());

        // Відкриваємо FIFO для запису. Це може блокуватися, якщо немає читача.
        fifo_fd = open(FIFO_NAME, O_WRONLY);
        if (fifo_fd == -1)
        {
            perror("Помилка відкриття FIFO для запису");
            exit(EXIT_FAILURE);
        }
        printf("FIFO відкрито для запису.\n");

        // Перенаправляємо stdout на FIFO
        if (dup2(fifo_fd, STDOUT_FILENO) == -1)
        {
            perror("Помилка перенаправлення stdout на FIFO");
            close(fifo_fd);
            exit(EXIT_FAILURE);
        }
        close(fifo_fd); // Закриваємо оригінальний дескриптор, оскільки dup2 його дублював

        // Запускаємо команду pstree -u -s
        execlp("pstree", "pstree", "-u", "-s", NULL);

        // Якщо execlp повернув управління сюди, значить сталася помилка
        perror("Помилка запуску pstree (execlp)");
        exit(EXIT_FAILURE);
    }

    // ---- Крок 2.3.3: Створити процес, який буде читати зі створеного каналу ----
    // Створюємо дочірній процес для читання з FIFO
    pid_reader = fork();
    if (pid_reader == -1)
    {
        perror("Помилка fork для процесу читання");
        waitpid(pid_writer, &status, 0); // Чекаємо письменника, якщо він був запущений
        unlink(FIFO_NAME);               // Видаляємо канал
        return EXIT_FAILURE;
    }

    if (pid_reader == 0)
    {
        // ---- КОД ДОЧІРНЬОГО ПРОЦЕСУ (ЧИТАЮЧИЙ) ----
        printf("\n--- Дочірній процес читання (PID: %d) починає роботу ---\n", getpid());
        printf("I'm reader process of %s with pid=%d\n", SURNAME, getpid());

        // Відкриваємо FIFO для читання. Це може блокуватися, якщо немає записувача.
        fifo_fd = open(FIFO_NAME, O_RDONLY);
        if (fifo_fd == -1)
        {
            perror("Помилка відкриття FIFO для читання");
            exit(EXIT_FAILURE);
        }
        printf("FIFO відкрито для читання.\n");

        // Читаємо дані з FIFO
        printf("Отримані дані з каналу:\n");
        bytes_read = 0;
        ssize_t current_read;
        while ((current_read = read(fifo_fd, buffer, sizeof(buffer))) > 0)
        {
            fwrite(buffer, 1, current_read, stdout); // Виводимо прочитані дані на stdout
            bytes_read += current_read;
        }

        if (current_read == -1)
        {
            perror("Помилка читання з FIFO");
            close(fifo_fd);
            exit(EXIT_FAILURE);
        }
        close(fifo_fd); // Закриваємо FIFO після читання
        printf("\nКінець читання з FIFO.\n");
        printf("Загальний розмір отриманого повідомлення: %zd байтів.\n", bytes_read);

        printf("--- Дочірній процес читання завершив роботу ---\n");
        exit(EXIT_SUCCESS);
    }

    // ---- КОД БАТЬКІВСЬКОГО ПРОЦЕСУ (ПРОДОВЖЕННЯ) ----
    // Батьківський процес чекає завершення обох дочірніх процесів
    printf("\nБатьківський процес (PID: %d) очікує завершення дочірніх...\n", getpid());

    waitpid(pid_writer, &status, 0);
    printf("Процес запису (PID: %d) завершив роботу.\n", pid_writer);

    waitpid(pid_reader, &status, 0);
    printf("Процес читання (PID: %d) завершив роботу.\n", pid_reader);

    // ---- Крок 2.3.4: Видалити іменований канал ----
    printf("\n2. Видалення іменованого каналу '%s'...\n", FIFO_NAME);
    if (unlink(FIFO_NAME) == -1)
    {
        perror("Помилка видалення іменованого каналу (unlink)");
        return EXIT_FAILURE;
    }
    printf("Канал '%s' успішно видалено.\n", FIFO_NAME);

    printf("\n--- Батьківський процес завершив роботу ---\n");
    return EXIT_SUCCESS;
}