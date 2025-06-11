#include <stdio.h>   // Для стандартних функцій вводу/виводу (printf)
#include <stdlib.h>  // Для стандартних функцій бібліотеки (exit)
#include <pthread.h> // Для роботи з потоками POSIX
#include <unistd.h>  // Для функції usleep()
#include <string.h>  // Для роботи з рядками (strcpy, snprintf)
#include <errno.h>   // Для errno, якщо потрібна детальна обробка помилок системних викликів (хоча тут прямо не використовується)

// --- Глобальна змінна для механізму "Змінні блокування" (Strict Alternation) ---
// Ця змінна керує, який потік має право увійти в критичну секцію.
// turn = 0 -> черга для T1
// turn = 1 -> черга для T2
volatile int turn = 0; // 'volatile' гарантує, що компілятор не буде оптимізувати доступ до цієї змінної

// --- Опис структурної змінної, який відповідає структурі реляційної таблиці "airplane" ---
// Це імітує один рядок (запис) з таблиці 'airplane'.
typedef struct
{
    int a_id;                  // Ідентифікатор літака (аналог первинного ключа 'a_id')
    char model[30];            // Модель літака (наприклад, 'Boeing-747', 'Airbus-A380')
    int capacity;              // Максимальна пасажиромісткість
    char status[20];           // Статус літака (наприклад, 'Active', 'Maintenance', 'Retired')
    double fuel_level_percent; // Рівень палива у відсотках
} AirplaneRecord;

// --- Спільна структурна змінна (один "рядок" таблиці), до якої будуть звертатися потоки ---
// Ініціалізуємо її початковими даними.
AirplaneRecord shared_airplane = {1, "Boeing-747", 400, "Active", 85.5};

// --- Функція затримки для імітації паралельної роботи потоків ---
void simulate_work(int milliseconds)
{
    // usleep приймає мікросекунди, тому множимо на 1000
    usleep(milliseconds * 1000);
}

// --- Функція для Потоку T1 ---
void *T1(void *arg)
{
    printf("T1 (Gavrylov): Запускаюся. Моя черга починається з %d.\n", turn);

    for (int i = 0; i < 3; i++)
    { // Кожен потік виконає операції кілька разів
        printf("\n--- T1 (Gavrylov): Ітерація %d ---\n", i + 1);

        // --- Вхідна секція алгоритму «Змінні блокування» ---
        // Потік T1 чекає, доки змінна 'turn' не стане 0.
        while (turn != 0)
        {
            printf("T1 (Gavrylov): Чекаю свою чергу (поточний turn = %d).\n", turn);
            simulate_work(70); // Невелика затримка для зменшення "busy-waiting"
        }

        // --- ВХІД У КРИТИЧНУ СЕКЦІЮ (Доступ до shared_airplane) ---
        printf("T1 (Gavrylov): ****** ВХОДЖУ В КРИТИЧНУ СЕКЦІЮ (Critical Region) ******\n");

        // --- Операція читання значення будь-якого елемента структури ---
        printf("T1 (Gavrylov): Читаю: a_id=%d, Model='%s', Capacity=%d, Status='%s', Fuel=%.1f%%\n",
               shared_airplane.a_id, shared_airplane.model, shared_airplane.capacity,
               shared_airplane.status, shared_airplane.fuel_level_percent);
        simulate_work(100); // Імітація часу на читання

        // --- Операція зміни значення вказаного елемента структури ---
        printf("T1 (Gavrylov): Змінюю дані літака...\n");
        shared_airplane.capacity -= 10;            // Зменшуємо місткість (можливо, для технічних потреб)
        shared_airplane.fuel_level_percent += 5.0; // Додаємо паливо
        snprintf(shared_airplane.model, sizeof(shared_airplane.model), "Boeing-747_mod_T1");
        snprintf(shared_airplane.status, sizeof(shared_airplane.status), "Active_T1");
        shared_airplane.a_id = 10 + i; // Змінюємо ID для відслідковування змін

        printf("T1 (Gavrylov): Оновлено: a_id=%d, Model='%s', Capacity=%d, Status='%s', Fuel=%.1f%%\n",
               shared_airplane.a_id, shared_airplane.model, shared_airplane.capacity,
               shared_airplane.status, shared_airplane.fuel_level_percent);
        simulate_work(120); // Імітація часу на запис

        // --- ВИХІД З КРИТИЧНОЇ СЕКЦІЇ ---
        printf("T1 (Gavrylov): ****** ВИХОДЖУ З КРИТИЧНОЇ СЕКЦІЇ ******\n");
        turn = 1; // Передаємо чергу Потоку T2
        printf("T1 (Gavrylov): Передав чергу T2. Поточний turn = %d. (Noncritical Region)\n", turn);
        simulate_work(150); // Робота поза критичною секцією
    }
    printf("T1 (Gavrylov): Завершив роботу.\n");
    pthread_exit(NULL);
}

// --- Функція для Потоку T2 ---
void *T2(void *arg)
{
    printf("T2 (Gavrylov): Запускаюся.\n");
    simulate_work(50); // Невелика початкова затримка, щоб T1, можливо, розпочав першим

    for (int i = 0; i < 3; i++)
    { // Кожен потік виконає операції кілька разів
        printf("\n--- T2 (Gavrylov): Ітерація %d ---\n", i + 1);

        // --- Вхідна секція алгоритму «Змінні блокування» ---
        // Потік T2 чекає, доки змінна 'turn' не стане 1.
        while (turn != 1)
        {
            printf("T2 (Gavrylov): Чекаю свою чергу (поточний turn = %d).\n", turn);
            simulate_work(70); // Невелика затримка для зменшення "busy-waiting"
        }

        // --- ВХІД У КРИТИЧНУ СЕКЦІЮ (Доступ до shared_airplane) ---
        printf("T2 (Gavrylov): ****** ВХОДЖУ В КРИТИЧНУ СЕКЦІЮ (Critical Region) ******\n");

        // --- Операція читання значення будь-якого елемента структури ---
        printf("T2 (Gavrylov): Читаю: a_id=%d, Model='%s', Capacity=%d, Status='%s', Fuel=%.1f%%\n",
               shared_airplane.a_id, shared_airplane.model, shared_airplane.capacity,
               shared_airplane.status, shared_airplane.fuel_level_percent);
        simulate_work(110); // Імітація часу на читання

        // --- Операція зміни значення вказаного елемента структури ---
        printf("T2 (Gavrylov): Змінюю дані літака...\n");
        shared_airplane.capacity += 5;             // Збільшуємо місткість
        shared_airplane.fuel_level_percent -= 2.0; // Витрачаємо паливо
        snprintf(shared_airplane.model, sizeof(shared_airplane.model), "Airbus-A380_upd_T2");
        snprintf(shared_airplane.status, sizeof(shared_airplane.status), "Flight_T2");
        shared_airplane.a_id = 20 + i; // Змінюємо ID для відслідковування змін

        printf("T2 (Gavrylov): Оновлено: a_id=%d, Model='%s', Capacity=%d, Status='%s', Fuel=%.1f%%\n",
               shared_airplane.a_id, shared_airplane.model, shared_airplane.capacity,
               shared_airplane.status, shared_airplane.fuel_level_percent);
        simulate_work(130); // Імітація часу на запис

        // --- ВИХІД З КРИТИЧНОЇ СЕКЦІЇ ---
        printf("T2 (Gavrylov): ****** ВИХОДЖУ З КРИТИЧНОЇ СЕКЦІЇ ******\n");
        turn = 0; // Передаємо чергу Потоку T1
        printf("T2 (Gavrylov): Передав чергу T1. Поточний turn = %d. (Noncritical Region)\n", turn);
        simulate_work(180); // Робота поза критичною секцією
    }
    printf("T2 (Gavrylov): Завершив роботу.\n");
    pthread_exit(NULL);
}

// --- Основна функція main ---
int main()
{
    pthread_t thread1, thread2;

    printf("Main (Gavrylov): Початковий стан shared_airplane: ID=%d, Model='%s', Capacity=%d, Status='%s', Fuel=%.1f%%\n",
           shared_airplane.a_id, shared_airplane.model, shared_airplane.capacity,
           shared_airplane.status, shared_airplane.fuel_level_percent);
    printf("Main (Gavrylov): Створюю потоки T1 та T2.\n");

    // Створення потоків
    if (pthread_create(&thread1, NULL, T1, NULL) != 0)
    {
        perror("Main (Gavrylov): Помилка створення потоку T1");
        return 1;
    }
    if (pthread_create(&thread2, NULL, T2, NULL) != 0)
    {
        perror("Main (Gavrylov): Помилка створення потоку T2");
        return 1;
    }

    // Очікування завершення потоків
    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);

    printf("\nMain (Gavrylov): Обидва потоки завершили роботу.\n");
    printf("Main (Gavrylov): Фінальний стан shared_airplane: ID=%d, Model='%s', Capacity=%d, Status='%s', Fuel=%.1f%%\n",
           shared_airplane.a_id, shared_airplane.model, shared_airplane.capacity,
           shared_airplane.status, shared_airplane.fuel_level_percent);

    return 0;
}
