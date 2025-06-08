#include <stdio.h>   // Для стандартних функцій вводу/виводу (printf)
#include <stdlib.h>  // Для стандартних функцій бібліотеки
#include <pthread.h> // Для роботи з потоками POSIX
#include <unistd.h>  // Для функції sleep() та usleep()
#include <string.h>  // Для роботи з рядками (strcpy, snprintf)

// --- Глобальна змінна для алгоритму «Строге чергування» ---
// Вказує, чий хід (0 або 1).
// turn = 0 означає, що T1 має право увійти в критичну секцію, turn = 1 для T2.
int turn = 0;

// --- Опис структурної змінної, який відповідає структурі реляційної таблиці ---
// Припустимо, що наша "таблиця" має поля ID, кількість та назву.
typedef struct
{
    int id;             // Ідентифікатор запису
    int quantity;       // Кількість одиниць (наприклад, товару)
    char item_name[50]; // Назва елемента
} inventory_item;

// --- Спільна структурна змінна, до якої будуть звертатися потоки ---
// Ініціалізуємо її початковими даними.
inventory_item shared_inventory = {101, 50, "Initial_Product"};

// --- Функція затримки для імітації паралельної роботи потоків ---
void simulate_work(int milliseconds)
{
    usleep(milliseconds * 1000); // usleep приймає мікросекунди
}

// --- Функція для Потоку T1 ---
void *T1(void *arg)
{
    printf("T1: Запускаюся. Моя черга починається з %d.\n", turn);

    for (int i = 0; i < 3; i++)
    { // Кожен потік виконає операції кілька разів
        printf("\n--- T1: Ітерація %d ---\n", i + 1);

        // --- Структури даних роботи алгоритму «Строге чергування» ---
        // P1 чекає, поки turn не стане 0 (його черга)
        while (turn != 0)
        {
            printf("T1: Чекаю свою чергу (поточний turn = %d).\n", turn);
            simulate_work(50); // Невелика затримка, щоб уникнути інтенсивного busy-waiting
        }

        // --- Вхід у Критичну Секцію для T1 ---
        printf("T1: ****** ВХОДЖУ В КРИТИЧНУ СЕКЦІЮ ******\n");

        // --- Операція читання значення будь-якого елемента структури ---
        printf("T1: Читаю: ID=%d, Кількість=%d, Назва='%s'\n",
               shared_inventory.id, shared_inventory.quantity, shared_inventory.item_name);

        simulate_work(100); // Імітація читання даних

        // --- Операція зміни значення вказаного елемента структури ---
        printf("T1: Змінюю значення кількості та назви...\n");
        shared_inventory.quantity += 5; // Збільшуємо кількість
        snprintf(shared_inventory.item_name, sizeof(shared_inventory.item_name), "Product_by_T1_%d", shared_inventory.quantity);
        shared_inventory.id = 1000 + i; // Змінюємо ID для відслідковування

        printf("T1: Змінено на: ID=%d, Кількість=%d, Назва='%s'\n",
               shared_inventory.id, shared_inventory.quantity, shared_inventory.item_name);

        simulate_work(100); // Імітація запису даних

        // --- Вихід з Критичної Секції для T1 ---
        printf("T1: ****** ВИХОДЖУ З КРИТИЧНОЇ СЕКЦІЇ ******\n");
        turn = 1; // Передаємо чергу T2
        printf("T1: Передав чергу T2. Поточний turn = %d.\n", turn);
        simulate_work(50); // Невелика затримка поза критичною секцією
    }
    printf("T1: Завершив роботу.\n");
    pthread_exit(NULL);
}

// --- Функція для Потоку T2 ---
void *T2(void *arg)
{
    printf("T2: Запускаюся. Моя черга починається з %d.\n", turn);

    // Додамо невелику початкову затримку для T2, щоб T1, швидше за все, розпочав першим
    simulate_work(20);

    for (int i = 0; i < 3; i++)
    { // Кожен потік виконає операції кілька разів
        printf("\n--- T2: Ітерація %d ---\n", i + 1);

        // --- Структури даних роботи алгоритму «Строге чергування» ---
        // P2 чекає, поки turn не стане 1 (його черга)
        while (turn != 1)
        {
            printf("T2: Чекаю свою чергу (поточний turn = %d).\n", turn);
            simulate_work(50); // Невелика затримка, щоб уникнути інтенсивного busy-waiting
        }

        // --- Вхід у Критичну Секцію для T2 ---
        printf("T2: ****** ВХОДЖУ В КРИТИЧНУ СЕКЦІЮ ******\n");

        // --- Операція читання значення будь-якого елемента структури ---
        printf("T2: Читаю: ID=%d, Кількість=%d, Назва='%s'\n",
               shared_inventory.id, shared_inventory.quantity, shared_inventory.item_name);

        simulate_work(120); // Імітація читання даних

        // --- Операція зміни значення вказаного елемента структури ---
        printf("T2: Змінюю значення кількості та назви...\n");
        shared_inventory.quantity *= 2; // Подвоюємо кількість
        snprintf(shared_inventory.item_name, sizeof(shared_inventory.item_name), "Item_by_T2_%d", shared_inventory.quantity);
        shared_inventory.id = 2000 + i; // Змінюємо ID для відслідковування

        printf("T2: Змінено на: ID=%d, Кількість=%d, Назва='%s'\n",
               shared_inventory.id, shared_inventory.quantity, shared_inventory.item_name);

        simulate_work(120); // Імітація запису даних

        // --- Вихід з Критичної Секції для T2 ---
        printf("T2: ****** ВИХОДЖУ З КРИТИЧНОЇ СЕКЦІЇ ******\n");
        turn = 0; // Передаємо чергу T1
        printf("T2: Передав чергу T1. Поточний turn = %d.\n", turn);
        simulate_work(50); // Невелика затримка поза критичною секцією
    }
    printf("T2: Завершив роботу.\n");
    pthread_exit(NULL);
}

// --- Основна функція main ---
int main()
{
    pthread_t thread1, thread2;

    printf("Main: Початковий стан shared_inventory: ID=%d, Кількість=%d, Назва='%s'\n",
           shared_inventory.id, shared_inventory.quantity, shared_inventory.item_name);
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
    printf("Main: Фінальний стан shared_inventory: ID=%d, Кількість=%d, Назва='%s'\n",
           shared_inventory.id, shared_inventory.quantity, shared_inventory.item_name);

    return 0;
}