#include <stdio.h>     // Для стандартних функцій вводу/виводу (printf)
#include <stdlib.h>    // Для стандартних функцій бібліотеки (exit)
#include <pthread.h>   // Для роботи з потоками POSIX
#include <unistd.h>    // Для функції usleep()
#include <string.h>    // Для роботи з рядками (snprintf)
#include <semaphore.h> // Для роботи з POSIX-семафорами
#include <fcntl.h>     // Для констант O_CREAT, O_EXCL, S_IRUSR, S_IWUSR, S_IRGRP, S_IWGRP
#include <sys/stat.h>  // Для S_IRUSR, S_IWUSR, S_IRGRP, S_IWGRP
#include <errno.h>     // <--- ДОДАЙТЕ ЦЕЙ РЯДОК

// --- Назва семафору (ваше прізвище транслітерацією) ---
// ВАЖЛИВО: Назви POSIX іменованих семафорів повинні починатися з '/'.
// Замініть 'your_surname_translit' на ваше прізвище транслітерацією.
#define SEM_NAME "/your_surname_translit_sem"

// --- Опис структурної змінної, який відповідає структурі реляційної таблиці ---
typedef struct
{
    int record_id;        // Ідентифікатор запису
    int value_data;       // Дані, які будуть змінювати потоки
    char record_name[50]; // Назва запису
} database_record;

// --- Спільна структурна змінна, до якої будуть звертатися потоки ---
database_record shared_record = {10, 100, "Initial_Record"};

// --- Глобальний покажчик на семафор ---
sem_t *binary_semaphore;

// --- Функція затримки для імітації паралельної роботи потоків ---
void simulate_work(int milliseconds)
{
    usleep(milliseconds * 1000); // usleep приймає мікросекунди
}

// --- Функція для Потоку T1 ---
void *T1(void *arg)
{
    printf("T1: Запускаюся.\n");

    for (int i = 0; i < 3; i++)
    { // Кожен потік виконає операції кілька разів
        printf("\n--- T1: Ітерація %d ---\n", i + 1);

        // --- Захоплення семафора (Операція P або wait) ---
        printf("T1: Спроба захопити семафор...\n");
        if (sem_wait(binary_semaphore) == -1)
        {
            perror("T1: Помилка sem_wait");
            pthread_exit((void *)1);
        }
        printf("T1: Семафор захоплено. ВХОДЖУ В КРИТИЧНУ СЕКЦІЮ.\n");

        // --- Критична Секція ---
        // --- Операція читання значення будь-якого елемента структури ---
        printf("T1: Читаю: ID=%d, Дані=%d, Назва='%s'\n",
               shared_record.record_id, shared_record.value_data, shared_record.record_name);
        simulate_work(50); // Імітація читання

        // --- Операція зміни значення вказаного елемента структури ---
        printf("T1: Змінюю дані та назву...\n");
        shared_record.value_data += 10;
        snprintf(shared_record.record_name, sizeof(shared_record.record_name), "T1_Changed_%d", shared_record.value_data);
        shared_record.record_id = 100 + i;
        printf("T1: Змінено на: ID=%d, Дані=%d, Назва='%s'\n",
               shared_record.record_id, shared_record.value_data, shared_record.record_name);
        simulate_work(50); // Імітація запису

        // --- Звільнення семафора (Операція V або post) ---
        printf("T1: Звільняю семафор. ВИХОДЖУ З КРИТИЧНОЇ СЕКЦІЇ.\n");
        if (sem_post(binary_semaphore) == -1)
        {
            perror("T1: Помилка sem_post");
            pthread_exit((void *)1);
        }
        simulate_work(150); // Робота поза критичною секцією
    }
    printf("T1: Завершив роботу.\n");
    pthread_exit(NULL);
}

// --- Функція для Потоку T2 ---
void *T2(void *arg)
{
    printf("T2: Запускаюся.\n");
    simulate_work(20); // Невелика затримка, щоб T1, можливо, розпочав першим

    for (int i = 0; i < 3; i++)
    { // Кожен потік виконає операції кілька разів
        printf("\n--- T2: Ітерація %d ---\n", i + 1);

        // --- Захоплення семафора (Операція P або wait) ---
        printf("T2: Спроба захопити семафор...\n");
        if (sem_wait(binary_semaphore) == -1)
        {
            perror("T2: Помилка sem_wait");
            pthread_exit((void *)1);
        }
        printf("T2: Семафор захоплено. ВХОДЖУ В КРИТИЧНУ СЕКЦІЮ.\n");

        // --- Критична Секція ---
        // --- Операція читання значення будь-якого елемента структури ---
        printf("T2: Читаю: ID=%d, Дані=%d, Назва='%s'\n",
               shared_record.record_id, shared_record.value_data, shared_record.record_name);
        simulate_work(60); // Імітація читання

        // --- Операція зміни значення вказаного елемента структури ---
        printf("T2: Змінюю дані та назву...\n");
        shared_record.value_data *= 2;
        snprintf(shared_record.record_name, sizeof(shared_record.record_name), "T2_Modified_%d", shared_record.value_data);
        shared_record.record_id = 200 + i;
        printf("T2: Змінено на: ID=%d, Дані=%d, Назва='%s'\n",
               shared_record.record_id, shared_record.value_data, shared_record.record_name);
        simulate_work(60); // Імітація запису

        // --- Звільнення семафора (Операція V або post) ---
        printf("T2: Звільняю семафор. ВИХОДЖУ З КРИТИЧНОЇ СЕКЦІЇ.\n");
        if (sem_post(binary_semaphore) == -1)
        {
            perror("T2: Помилка sem_post");
            pthread_exit((void *)1);
        }
        simulate_work(120); // Робота поза критичною секцією
    }
    printf("T2: Завершив роботу.\n");
    pthread_exit(NULL);
}

// --- Основна функція main ---
int main()
{
    pthread_t thread1, thread2;

    printf("Main: Початковий стан shared_record: ID=%d, Дані=%d, Назва='%s'\n",
           shared_record.record_id, shared_record.value_data, shared_record.record_name);

    // --- Функції керування семафором: Створення/Відкриття семафора ---
    // O_CREAT: Створити семафор, якщо він не існує.
    // O_EXCL: Якщо семафор вже існує, sem_open() поверне помилку (EEXIST).
    //         Це допомагає уникнути використання старого, "застряглого" семафора.
    // 0660: Права доступу: читання/запис власнику (0600) + читання/запис групі (0060)
    // 1: Початкове значення семафора (двійковий семафор = 0 або 1).
    //    1 означає, що ресурс доступний; 0 означає, що заблокований.
    binary_semaphore = sem_open(SEM_NAME, O_CREAT | O_EXCL, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP, 1);
    if (binary_semaphore == SEM_FAILED)
    {
        // Якщо семафор вже існує (EEXIST), спробуємо просто відкрити його.
        if (errno == EEXIST)
        {
            printf("Main: Семафор '%s' вже існує. Спроба відкрити існуючий семафор.\n", SEM_NAME);
            binary_semaphore = sem_open(SEM_NAME, 0); // Просто відкриваємо існуючий семафор
            if (binary_semaphore == SEM_FAILED)
            {
                perror("Main: Критична помилка при відкритті існуючого семафора");
                return 1;
            }
        }
        else
        {
            perror("Main: Критична помилка при створенні семафора");
            return 1;
        }
    }
    else
    {
        printf("Main: Семафор '%s' успішно створено та відкрито.\n", SEM_NAME);
    }

    printf("Main: Створюю потоки T1 та T2.\n");

    // Створення потоків
    if (pthread_create(&thread1, NULL, T1, NULL) != 0)
    {
        perror("Main: Помилка створення потоку T1");
        // Перед виходом намагаємося закрити і розіменувати семафор
        sem_close(binary_semaphore);
        sem_unlink(SEM_NAME);
        return 1;
    }
    if (pthread_create(&thread2, NULL, T2, NULL) != 0)
    {
        perror("Main: Помилка створення потоку T2");
        // Перед виходом намагаємося закрити і розіменувати семафор
        sem_close(binary_semaphore);
        sem_unlink(SEM_NAME);
        return 1;
    }

    // Очікування завершення потоків
    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);

    printf("\nMain: Обидва потоки завершили роботу.\n");
    printf("Main: Фінальний стан shared_record: ID=%d, Дані=%d, Назва='%s'\n",
           shared_record.record_id, shared_record.value_data, shared_record.record_name);

    // --- Функції керування семафором: Закриття та Знищення семафора ---
    printf("Main: Закриваю семафор...\n");
    if (sem_close(binary_semaphore) == -1)
    {
        perror("Main: Помилка sem_close");
    }

    printf("Main: Видаляю (unlinking) семафор...\n");
    // sem_unlink видаляє ім'я семафора з файлової системи.
    // Сам семафор буде знищений, коли всі процеси, що його використовують, закриють його.
    if (sem_unlink(SEM_NAME) == -1)
    {
        perror("Main: Помилка sem_unlink");
    }
    else
    {
        printf("Main: Семафор '%s' успішно видалено.\n", SEM_NAME);
    }

    return 0;
}