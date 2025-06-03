// thread_example.c
// Програма, що створює два потоки, які виводять повідомлення.

#include <stdio.h>   // Для printf
#include <stdlib.h>  // Для EXIT_SUCCESS, EXIT_FAILURE
#include <pthread.h> // Для pthread_create, pthread_join, pthread_t
#include <unistd.h>  // Для usleep (не обов'язково, але допомагає побачити перемикання)

#define SURNAME "Vyacheslavovich"          // Ваше прізвище транслітерацією
#define VARIANT_NUMBER 5                   // Номер вашого варіанту (n)
#define MESSAGE_COUNT (VARIANT_NUMBER + 5) // Кількість виводів на потік (n+5)

// Функція, яка буде виконуватися в кожному потоці
// Аргумент void *arg дозволяє передати дані в потік.
void *thread_function(void *arg)
{
    int thread_num = *(int *)arg; // Отримуємо номер потоку
    int i;

    printf("Потік %d (PID: %d) починає роботу. Я: %s.\n", thread_num, getpid(), SURNAME);

    // Виводимо повідомлення (n+5) разів
    for (i = 1; i <= MESSAGE_COUNT; i++)
    {
        printf("Повідомлення від %s, Потік №%d: Ітерація %d/%d\n", SURNAME, thread_num, i, MESSAGE_COUNT);
        usleep(100000); // Невелика затримка для демонстрації конкурентності (100 мс)
    }

    printf("Потік %d (PID: %d) завершує роботу.\n", thread_num, getpid());

    pthread_exit(NULL); // Завершення потоку
}

int main()
{
    pthread_t thread1, thread2; // Змінні для ідентифікаторів потоків
    int thread1_arg = 1;        // Аргумент для першого потоку
    int thread2_arg = 2;        // Аргумент для другого потоку

    printf("--- Головний потік (PID: %d) починає роботу ---\n", getpid());

    // Створюємо перший потік
    if (pthread_create(&thread1, NULL, thread_function, (void *)&thread1_arg) != 0)
    {
        perror("Помилка створення першого потоку");
        return EXIT_FAILURE;
    }

    // Створюємо другий потік
    if (pthread_create(&thread2, NULL, thread_function, (void *)&thread2_arg) != 0)
    {
        perror("Помилка створення другого потоку");
        return EXIT_FAILURE;
    }

    // Головний потік очікує завершення дочірніх потоків
    printf("Головний потік очікує завершення дочірніх потоків...\n");

    if (pthread_join(thread1, NULL) != 0)
    {
        perror("Помилка очікування першого потоку");
        return EXIT_FAILURE;
    }

    if (pthread_join(thread2, NULL) != 0)
    {
        perror("Помилка очікування другого потоку");
        return EXIT_FAILURE;
    }

    printf("--- Головний потік завершив роботу. Обидва дочірні потоки завершені ---\n");

    return EXIT_SUCCESS;
}