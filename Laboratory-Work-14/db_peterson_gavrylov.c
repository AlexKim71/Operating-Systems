#include <stdio.h>   // Для стандартних функцій вводу/виводу (printf)
#include <stdlib.h>  // Для стандартних функцій бібліотеки
#include <pthread.h> // Для роботи з потоками POSIX
#include <unistd.h>  // Для функції sleep() та usleep()
#include <string.h>  // Для роботи з рядками (strcpy, snprintf)

// --- Глобальні змінні для алгоритму Петерсона ---
// flag[0] = true, якщо P0 хоче увійти в критичну секцію
// flag[1] = true, якщо P1 хоче увійти в критичну секцію
volatile int flag[2] = {0, 0}; // Використання volatile гарантує, що компілятор не буде оптимізувати доступ до цих змінних

// turn: Вказує, чий хід, якщо обидва потоки хочуть увійти одночасно.
// turn = 0 означає, що P0 (T1) має пріоритет
// turn = 1 означає, що P1 (T2) має пріоритет
volatile int turn = 0; // turn = 0 означає, що P0 (T1) має право першим спробувати

// --- Опис структурної змінної, який відповідає структурі реляційної таблиці ---
typedef struct
{
    int record_id;         // Ідентифікатор запису
    int current_value;     // Значення, яке будуть змінювати потоки
    char description[100]; // Опис запису
} database_record;

// --- Спільна структурна змінна, до якої будуть звертатися потоки ---
database_record shared_db_record = {1, 100, "Initial database record"};

// --- Функція затримки для імітації паралельної роботи потоків ---
void simulate_work(int milliseconds)
{
    usleep(milliseconds * 1000); // usleep приймає мікросекунди
}

// --- Функція для Потоку T1 (відповідає P0 в алгоритмі Петерсона) ---
void *T1(void *arg)
{
    int thread_id = 0; // T1 відповідає P0
    printf("T1 (ID=%d): Запускаюся.\n", thread_id);

    for (int i = 0; i < 3; i++)
    { // Кожен потік виконає операції кілька разів
        printf("\n--- T1 (ID=%d): Ітерація %d ---\n", thread_id, i + 1);

        // --- Вхідна секція алгоритму Петерсона для P0 ---
        flag[thread_id] = 1;  // P0 заявляє про бажання увійти
        turn = 1 - thread_id; // P0 передає чергу P1 (тобто turn = 1)
        printf("T1 (ID=%d): Встановив flag[%d]=1 та turn=%d. Чекаю...\n", thread_id, thread_id, turn);

        // Чекаємо, якщо P1 теж хоче увійти, і черга належить P1
        while (flag[1 - thread_id] == 1 && turn == (1 - thread_id))
        {
            // Active waiting, але з умовою, що запобігає deadlock
            // printf("T1 (ID=%d): Очікую (flag[%d]=%d, turn=%d).\n", thread_id, 1-thread_id, flag[1-thread_id], turn);
            simulate_work(20); // Невелика затримка для зменшення навантаження на CPU
        }

        // --- Критична Секція ---
        printf("T1 (ID=%d): ****** ВХОДЖУ В КРИТИЧНУ СЕКЦІЮ ******\n", thread_id);

        // --- Операція читання значення будь-якого елемента структури ---
        printf("T1 (ID=%d): Читаю: ID=%d, Значення=%d, Опис='%s'\n",
               thread_id, shared_db_record.record_id, shared_db_record.current_value, shared_db_record.description);

        simulate_work(50); // Імітація читання даних

        // --- Операція зміни значення вказаного елемента структури ---
        printf("T1 (ID=%d): Змінюю значення та опис...\n", thread_id);
        shared_db_record.current_value += 1; // Змінюємо значення
        snprintf(shared_db_record.description, sizeof(shared_db_record.description), "Modified by T1 (%d)", shared_db_record.current_value);
        shared_db_record.record_id = 1000 + i;

        printf("T1 (ID=%d): Змінено на: ID=%d, Значення=%d, Опис='%s'\n",
               thread_id, shared_db_record.record_id, shared_db_record.current_value, shared_db_record.description);

        simulate_work(50); // Імітація запису даних

        // --- Вихідна секція алгоритму Петерсона для P0 ---
        printf("T1 (ID=%d): ****** ВИХОДЖУ З КРИТИЧНОЇ СЕКЦІЇ ******\n", thread_id);
        flag[thread_id] = 0; // P0 заявляє, що більше не хоче увійти
        printf("T1 (ID=%d): Встановив flag[%d]=0.\n", thread_id, thread_id);

        simulate_work(100); // Робота в некритичній секції
    }
    printf("T1 (ID=%d): Завершив роботу.\n", thread_id);
    pthread_exit(NULL);
}

// --- Функція для Потоку T2 (відповідає P1 в алгоритмі Петерсона) ---
void *T2(void *arg)
{
    int thread_id = 1; // T2 відповідає P1
    printf("T2 (ID=%d): Запускаюся.\n", thread_id);

    // Додамо невелику початкову затримку для T2, щоб T1, швидше за все, розпочав першим,
    // і алгоритм Петерсона міг продемонструвати свою логіку.
    simulate_work(10);

    for (int i = 0; i < 3; i++)
    { // Кожен потік виконає операції кілька разів
        printf("\n--- T2 (ID=%d): Ітерація %d ---\n", thread_id, i + 1);

        // --- Вхідна секція алгоритму Петерсона для P1 ---
        flag[thread_id] = 1;  // P1 заявляє про бажання увійти
        turn = 1 - thread_id; // P1 передає чергу P0 (тобто turn = 0)
        printf("T2 (ID=%d): Встановив flag[%d]=1 та turn=%d. Чекаю...\n", thread_id, thread_id, turn);

        // Чекаємо, якщо P0 теж хоче увійти, і черга належить P0
        while (flag[1 - thread_id] == 1 && turn == (1 - thread_id))
        {
            // Active waiting
            // printf("T2 (ID=%d): Очікую (flag[%d]=%d, turn=%d).\n", thread_id, 1-thread_id, flag[1-thread_id], turn);
            simulate_work(20); // Невелика затримка для зменшення навантаження на CPU
        }

        // --- Критична Секція ---
        printf("T2 (ID=%d): ****** ВХОДЖУ В КРИТИЧНУ СЕКЦІЮ ******\n", thread_id);

        // --- Операція читання значення будь-якого елемента структури ---
        printf("T2 (ID=%d): Читаю: ID=%d, Значення=%d, Опис='%s'\n",
               thread_id, shared_db_record.record_id, shared_db_record.current_value, shared_db_record.description);

        simulate_work(60); // Імітація читання даних

        // --- Операція зміни значення вказаного елемента структури ---
        printf("T2 (ID=%d): Змінюю значення та опис...\n", thread_id);
        shared_db_record.current_value *= 2; // Множимо значення
        snprintf(shared_db_record.description, sizeof(shared_db_record.description), "Modified by T2 (%d)", shared_db_record.current_value);
        shared_db_record.record_id = 2000 + i;

        printf("T2 (ID=%d): Змінено на: ID=%d, Значення=%d, Опис='%s'\n",
               thread_id, shared_db_record.record_id, shared_db_record.current_value, shared_db_record.description);

        simulate_work(60); // Імітація запису даних

        // --- Вихідна секція алгоритму Петерсона для P1 ---
        printf("T2 (ID=%d): ****** ВИХОДЖУ З КРИТИЧНОЇ СЕКЦІЇ ******\n", thread_id);
        flag[thread_id] = 0; // P1 заявляє, що більше не хоче увійти
        printf("T2 (ID=%d): Встановив flag[%d]=0.\n", thread_id, thread_id);

        simulate_work(100); // Робота в некритичній секції
    }
    printf("T2 (ID=%d): Завершив роботу.\n", thread_id);
    pthread_exit(NULL);
}

// --- Основна функція main ---
int main()
{
    pthread_t thread1, thread2;

    printf("Main: Початковий стан shared_db_record: ID=%d, Значення=%d, Опис='%s'\n",
           shared_db_record.record_id, shared_db_record.current_value, shared_db_record.description);
    printf("Main: Створюю потоки T1 та T2.\n");

    // Створення потоків
    if (pthread_create(&thread1, NULL, T1, NULL) != 0)
    {
        perror("Помилка створення потоку T1");
        return 1;
    }
    if (pthread_create(&thread2, NULL, T2, NULL) != 0)
    {
        perror("Помилка створення потоку T2");
        return 1;
    }

    // Очікування завершення потоків
    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);

    printf("\nMain: Обидва потоки завершили роботу.\n");
    printf("Main: Фінальний стан shared_db_record: ID=%d, Значення=%d, Опис='%s'\n",
           shared_db_record.record_id, shared_db_record.current_value, shared_db_record.description);

    return 0;
}