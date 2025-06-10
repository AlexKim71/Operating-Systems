#include <stdio.h>    // Для стандартних функцій вводу/виводу (printf)
#include <stdlib.h>   // Для стандартних функцій бібліотеки
#include <pthread.h>  // Для роботи з потоками POSIX
#include <unistd.h>   // Для функції sleep() та usleep()
#include <errno.h>    // Для обробки помилок, зокрема EEXIST
#include <string.h>   // Для роботи з рядками (strcpy, snprintf)
#include <sys/stat.h> // Хоча не використовується явно в цьому коді для S_IRUSR тощо, корисно для загального контексту файлових операцій

// --- Глобальна змінна для механізму "Змінні блокування" (Strict Alternation) ---
// Вказує, чий хід (0 або 1).
// turn = 0 означає, що T1 має право увійти в критичну секцію, turn = 1 для T2.
volatile int turn = 0; // Використовуємо volatile для змінної turn для коректної роботи між потоками

// --- Опис структурної змінної, який відповідає структурі реляційної таблиці ---
// Це є більш відповідною імітацією запису реляційної таблиці.
typedef struct {
    int id;                // Унікальний ідентифікатор запису (як первинний ключ)
    char client_name[50];  // Ім'я клієнта або інший текстовий опис
    double amount;         // Числове значення (наприклад, сума транзакції)
    int status_code;       // Код статусу (наприклад, 1 - активний, 0 - завершений)
} TransactionRecord;

// --- Спільна структурна змінна, до якої будуть звертатися потоки ---
// Ініціалізуємо її початковими даними.
TransactionRecord shared_record = {1001, "Initial Client", 500.0, 1};

// --- Функція затримки для імітації паралельної роботи ---
void simulate_work(int milliseconds) {
    usleep(milliseconds * 1000); // usleep приймає мікросекунди
}

// --- Функція для Потоку T1 ---
void *T1(void *arg) {
    printf("T1: Запускаюся. Моя черга починається з %d.\n", turn);

    for (int i = 0; i < 3; i++) { // Кожен потік виконає операції 3 рази
        printf("\n--- T1: Ітерація %d ---\n", i + 1);

        // --- Вхідна секція алгоритму «Змінні блокування» (Strict Alternation) ---
        // Чекаємо, поки turn не стане 0 (моя черга)
        while (turn != 0) {
            printf("T1: Чекаю свою чергу (поточний turn = %d).\n", turn);
            simulate_work(50); // Невелика затримка, щоб уникнути інтенсивного busy-waiting
        }

        // --- ВХІД У КРИТИЧНУ СЕКЦІЮ ---
        printf("T1: ****** ВХОДЖУ В КРИТИЧНУ СЕКЦІЮ ******\n");

        // --- Операція читання значення будь-якого елемента структури ---
        printf("T1: Читаю shared_record: ID=%d, Клієнт='%s', Сума=%.2f, Статус=%d\n",
               shared_record.id, shared_record.client_name, shared_record.amount, shared_record.status_code);
        simulate_work(100); // Імітація читання даних

        // --- Операція зміни значення вказаного елемента структури ---
        printf("T1: Змінюю shared_record.amount та shared_record.status_code...\n");
        shared_record.amount += 10.5; // Змінюємо суму
        shared_record.status_code = 1; // Встановлюємо статус на "активний"
        snprintf(shared_record.client_name, sizeof(shared_record.client_name), "Client_T1_Upd_%d", i);
        shared_record.id = 1000 + i; // Змінюємо ID для відслідковування#include <stdio.h>   // Для стандартних функцій вводу/виводу (printf)
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
}

        printf("T1: Оновлено shared_record: ID=%d, Клієнт='%s', Сума=%.2f, Статус=%d\n",
               shared_record.id, shared_record.client_name, shared_record.amount, shared_record.status_code);
        simulate_work(100); // Імітація запису даних

        // --- ВИХІД З КРИТИЧНОЇ СЕКЦІЇ ---
        printf("T1: ****** ВИХОДЖУ З КРИТИЧНОЇ СЕКЦІЇ ******\n");
        turn = 1; // Передаємо чергу T2
        printf("T1: Передав чергу T2. Поточний turn = %d.\n", turn);
        simulate_work(50); // Невелика затримка поза критичною секцією
    }
    printf("T1: Завершив роботу.\n");
    pthread_exit(NULL);
}

// --- Функція для Потоку T2 ---
void *T2(void *arg) {
    printf("T2: Запускаюся.\n");
    simulate_work(20); // Додамо невелику початкову затримку для T2

    for (int i = 0; i < 3; i++) { // Кожен потік виконає операції 3 рази
        printf("\n--- T2: Ітерація %d ---\n", i + 1);

        // --- Вхідна секція алгоритму «Змінні блокування» (Strict Alternation) ---
        // Чекаємо, поки turn не стане 1 (моя черга)
        while (turn != 1) {
            printf("T2: Чекаю свою чергу (поточний turn = %d).\n", turn);
            simulate_work(50); // Невелика затримка
        }

        // --- ВХІД У КРИТИЧНУ СЕКЦІЮ ---
        printf("T2: ****** ВХОДЖУ В КРИТИЧНУ СЕКЦІЮ ******\n");

        // --- Операція читання значення будь-якого елемента структури ---
        printf("T2: Читаю shared_record: ID=%d, Клієнт='%s', Сума=%.2f, Статус=%d\n",
               shared_record.id, shared_record.client_name, shared_record.amount, shared_record.status_code);
        simulate_work(120); // Імітація читання даних

        // --- Операція зміни значення вказаного елемента структури ---
        printf("T2: Змінюю shared_record.amount та shared_record.status_code...\n");
        shared_record.amount *= 0.9; // Зменшуємо суму на 10%
        shared_record.status_code = 0; // Встановлюємо статус на "завершений"
        snprintf(shared_record.client_name, sizeof(shared_record.client_name), "Client_T2_Upd_%d", i);
        shared_record.id = 2000 + i; // Змінюємо ID для відслідковування

        printf("T2: Оновлено shared_record: ID=%d, Клієнт='%s', Сума=%.2f, Статус=%d\n",
               shared_record.id, shared_record.client_name, shared_record.amount, shared_record.status_code);
        simulate_work(120); // Імітація запису даних

        // --- ВИХІД З КРИТИЧНОЇ СЕКЦІЇ ---
        printf("T2: ****** ВИХОДЖУ З КРИТИЧНОЇ СЕКЦІЇ ******\n");
        turn = 0; // Передаємо чергу T1
        printf("T2: Передав чергу T1. Поточний turn = %d.\n", turn);
        simulate_work(50); // Невелика затримка поза критичною секцією
    }
    printf("T2: Завершив роботу.\n");
    pthread_exit(NULL);
}

// --- Основна функція main ---
int main() {
    pthread_t thread1, thread2;

    printf("Main: Початковий стан shared_record: ID=%d, Клієнт='%s', Сума=%.2f, Статус=%d\n",
           shared_record.id, shared_record.client_name, shared_record.amount, shared_record.status_code); // Виправлено: shared_record.status_code замість shared_record.record_id
    printf("Main: Створюю потоки T1 та T2.\n");

    // Створення потоків
    if (pthread_create(&thread1, NULL, T1, NULL) != 0) {
        perror("Помилка створення потоку T1");
        return 1;
    }
    if (pthread_create(&thread2, NULL, T2, NULL) != 0) {
        perror("Помилка створення потоку T2");
        return 1;
    }

    // Очікування завершення потоків
    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);

    printf("\nMain: Обидва потоки завершили роботу.\n");
    printf("Main: Фінальний стан shared_record: ID=%d, Клієнт='%s', Сума=%.2f, Статус=%d\n",
           shared_record.id, shared_record.client_name, shared_record.amount, shared_record.status_code);

    return 0;
}
