#include <stdio.h>   // Для стандартних функцій вводу/виводу (printf)
#include <stdlib.h>  // Для стандартних функцій бібліотеки
#include <pthread.h> // Для роботи з потоками POSIX
#include <unistd.h>  // Для функції sleep() та usleep()
#include <errno.h>   // Для обробки помилок (хоча в цьому прикладі явно не використовується)
#include <string.h>  // Для роботи з рядками (strcpy, snprintf)

// --- Глобальна змінна для механізму "Змінні блокування" (Strict Alternation) ---
// Вказує, чий хід (0 або 1).
// turn = 0 означає, що T1 має право увійти в критичну секцію, turn = 1 для T2.
int turn = 0;

// --- Опис структурної змінної, який відповідає структурі реляційної таблиці ---
// Припустимо, що наша "таблиця" має поле "bill_count" та "name"
typedef struct
{
    int id;         // Наприклад, ID рахунку
    int bill_count; // Кількість чогось у рахунку
    char name[20];  // Ім'я власника рахунку
} bill_struct;

// --- Спільна структурна змінна, до якої будуть звертатися потоки ---
// Ініціалізуємо її за прикладом з рисунка, але з полем bill_count, що відповідає
// вимогам завдання (читання/зміна bill_count).
bill_struct bill[2] = {{1, 100, "Gavrylov_Initial"}}; // Використовуємо bill[0] як наш єдиний запис для доступу

// --- Функція затримки для імітації паралельної роботи ---
void simulate_work(int seconds)
{
    sleep(seconds); // Затримка в секундах
}

// --- Функція для Потоку T1 ---
void *T1(void *arg)
{
    printf("T1 (Gavrylov): Запускаюся. Моя черга починається з %d.\n", turn);

    for (int i = 0; i < 3; i++)
    { // Повторюємо операції 3 рази
        printf("\nT1 (Gavrylov): Ітерація %d.\n", i + 1);

        // --- Структури даних роботи алгоритму «Змінні блокування» ---
        // Чекаємо, поки turn не стане 0 (моя черга)
        while (turn != 0)
        {
            // printf("T1 (Gavrylov): Чекаю свою чергу (turn = %d).\n", turn);
            simulate_work(1); // Невелика затримка, щоб уникнути "busy waiting" повністю
        }

        // --- Вхід у Критичну Секцію ---
        printf("T1 (Gavrylov): ВХОДЖУ В КРИТИЧНУ СЕКЦІЮ.\n");
        printf("T1 (Gavrylov): Critical Region\n");

        // --- Операція читання значення будь-якого елемента структури ---
        printf("T1 (Gavrylov): Читаю bill[0].bill_count = %d\n", bill[0].bill_count);
        simulate_work(1); // Імітація роботи

        // --- Операція зміни значення вказаного елемента структури ---
        bill[0].bill_count = 10; // Змінюємо значення bill_count
        printf("T1 (Gavrylov): Write [bill_count]=%d\\n", bill[0].bill_count);
        // Змінюємо також інші поля для демонстрації доступу до структури
        strcpy(bill[0].name, "Gavrylov_T1_Changed");
        bill[0].id = 101;
        printf("T1 (Gavrylov): Змінено на ID=%d, Name=%s, Count=%d\n", bill[0].id, bill[0].name, bill[0].bill_count);

        simulate_work(1); // Імітація роботи

        // --- Вихід з Критичної Секції ---
        printf("T1 (Gavrylov): ВИХОДЖУ З КРИТИЧНОЇ СЕКЦІЇ.\n");
        turn = 1; // Передаємо чергу T2
        printf("T1 (Gavrylov): Noncritical Region\n");
        simulate_work(1); // Імітація роботи в некритичній секції
    }
    printf("T1 (Gavrylov): Завершую роботу.\n");
    pthread_exit(NULL);
}

// --- Функція для Потоку T2 ---
void *T2(void *arg)
{
    printf("T2 (Gavrylov): Запускаюся. Моя черга починається з %d.\n", turn);

    // simulate_work(1); // T2 стартує пізніше T1 (як на рисунку, але можна і без)

    for (int i = 0; i < 3; i++)
    { // Повторюємо операції 3 рази
        printf("\nT2 (Gavrylov): Ітерація %d.\n", i + 1);

        // --- Структури даних роботи алгоритму «Змінні блокування» ---
        // Чекаємо, поки turn не стане 1 (моя черга)
        while (turn != 1)
        {
            // printf("T2 (Gavrylov): Чекаю свою чергу (turn = %d).\n", turn);
            simulate_work(1); // Невелика затримка, щоб уникнути "busy waiting" повністю
        }

        // --- Вхід у Критичну Секцію ---
        printf("T2 (Gavrylov): ВХОДЖУ В КРИТИЧНУ СЕКЦІЮ.\n");
        printf("T2 (Gavrylov): Critical Region\n");

        // --- Операція читання значення будь-якого елемента структури ---
        printf("T2 (Gavrylov): Read[bill_count]=%d\n", bill[0].bill_count);
        simulate_work(1); // Імітація роботи

        // --- Операція зміни значення вказаного елемента структури ---
        bill[0].bill_count += 5; // Змінюємо значення bill_count
        printf("T2 (Gavrylov): Write [bill_count]=%d\\n", bill[0].bill_count);
        // Змінюємо також інші поля для демонстрації доступу до структури
        strcpy(bill[0].name, "Gavrylov_T2_Modified");
        bill[0].id = 202;
        printf("T2 (Gavrylov): Змінено на ID=%d, Name=%s, Count=%d\n", bill[0].id, bill[0].name, bill[0].bill_count);

        simulate_work(1); // Імітація роботи

        // --- Вихід з Критичної Секції ---
        printf("T2 (Gavrylov): ВИХОДЖУ З КРИТИЧНОЇ СЕКЦІЇ.\n");
        turn = 0; // Передаємо чергу T1
        printf("T2 (Gavrylov): Noncritical Region\n");
        simulate_work(1); // Імітація роботи в некритичній секції
    }
    printf("T2 (Gavrylov): Завершую роботу.\n");
    pthread_exit(NULL);
}

// --- Основна функція main ---
int main()
{
    pthread_t T1_thread, T2_thread;

    printf("Main (Gavrylov): Початковий стан bill[0]: ID=%d, Name=%s, Count=%d\n",
           bill[0].id, bill[0].name, bill[0].bill_count);
    printf("Main (Gavrylov): Створюю потоки T1 та T2.\n");

    // Створення потоків
    // pthread_create(&T1_thread, NULL, T1, NULL); // Як на рисунку
    // pthread_create(&T2_thread, NULL, T2, NULL); // Як на рисунку
    // Якщо хочемо, щоб T1 почав першим, turn = 0 спочатку
    if (pthread_create(&T1_thread, NULL, T1, NULL) != 0)
    {
        perror("Помилка створення потоку T1");
        return 1;
    }
    if (pthread_create(&T2_thread, NULL, T2, NULL) != 0)
    {
        perror("Помилка створення потоку T2");
        return 1;
    }

    // Очікування завершення потоків
    pthread_join(T1_thread, NULL);
    pthread_join(T2_thread, NULL);

    printf("\nMain (Gavrylov): Обидва потоки завершили роботу.\n");
    printf("Main (Gavrylov): Фінальний стан bill[0]: ID=%d, Name=%s, Count=%d\n",
           bill[0].id, bill[0].name, bill[0].bill_count);

    return 0;
}