#include <stdio.h>     // Для стандартних функцій вводу/виводу (printf)
#include <stdlib.h>    // Для стандартних функцій бібліотеки (exit)
#include <pthread.h>   // Для роботи з потоками POSIX та м'ютексами
#include <unistd.h>    // Для функції usleep()
#include <string.h>    // Для роботи з рядками (snprintf)

// --- Опис структурної змінної, який відповідає структурі реляційної таблиці ---
typedef struct {
    int product_id;       // Ідентифікатор продукту
    int stock_level;      // Рівень запасу (кількість на складі)
    char product_name[60]; // Назва продукту
} product_inventory;

// --- Спільна структурна змінна, до якої будуть звертатися потоки ---
product_inventory shared_product = {1001, 500, "Initial_Gadget_Pro"};

// --- Глобальний м'ютекс ---
pthread_mutex_t shared_data_mutex;

// --- Функція затримки для імітації паралельної роботи потоків ---
void simulate_work(int milliseconds) {
    usleep(milliseconds * 1000); // usleep приймає мікросекунди
}

// --- Функція для Потоку T1 ---
void *T1(void *arg) {
    printf("T1: Запускаюся.\n");

    for (int i = 0; i < 3; i++) { // Кожен потік виконає операції кілька разів
        printf("\n--- T1: Ітерація %d ---\n", i + 1);

        // --- Захоплення м'ютекса (pthread_mutex_lock) ---
        printf("T1: Спроба захопити м'ютекс...\n");
        if (pthread_mutex_lock(&shared_data_mutex) != 0) {
            perror("T1: Помилка pthread_mutex_lock");
            pthread_exit((void*)1);
        }
        printf("T1: М'ютекс захоплено. ВХОДЖУ В КРИТИЧНУ СЕКЦІЮ.\n");

        // --- Критична Секція ---
        // --- Операція читання значення будь-якого елемента структури ---
        printf("T1: Читаю: ID=%d, Залишок=%d, Назва='%s'\n",
               shared_product.product_id, shared_product.stock_level, shared_product.product_name);
        simulate_work(50); // Імітація читання

        // --- Операція зміни значення вказаного елемента структури ---
        printf("T1: Змінюю рівень запасу та назву...\n");
        shared_product.stock_level -= 10; // Зменшуємо запас
        snprintf(shared_product.product_name, sizeof(shared_product.product_name), "Gadget_by_T1_%d", shared_product.stock_level);
        shared_product.product_id = 1000 + i;
        printf("T1: Змінено на: ID=%d, Залишок=%d, Назва='%s'\n",
               shared_product.product_id, shared_product.stock_level, shared_product.product_name);
        simulate_work(50); // Імітація запису

        // --- Звільнення м'ютекса (pthread_mutex_unlock) ---
        printf("T1: Звільняю м'ютекс. ВИХОДЖУ З КРИТИЧНОЇ СЕКЦІЇ.\n");
        if (pthread_mutex_unlock(&shared_data_mutex) != 0) {
            perror("T1: Помилка pthread_mutex_unlock");
            pthread_exit((void*)1);
        }
        simulate_work(150); // Робота поза критичною секцією
    }
    printf("T1: Завершив роботу.\n");
    pthread_exit(NULL);
}

// --- Функція для Потоку T2 ---
void *T2(void *arg) {
    printf("T2: Запускаюся.\n");
    simulate_work(20); // Невелика затримка, щоб T1, можливо, розпочав першим

    for (int i = 0; i < 3; i++) { // Кожен потік виконає операції кілька разів
        printf("\n--- T2: Ітерація %d ---\n", i + 1);

        // --- Захоплення м'ютекса (pthread_mutex_lock) ---
        printf("T2: Спроба захопити м'ютекс...\n");
        if (pthread_mutex_lock(&shared_data_mutex) != 0) {
            perror("T2: Помилка pthread_mutex_lock");
            pthread_exit((void*)1);
        }
        printf("T2: М'ютекс захоплено. ВХОДЖУ В КРИТИЧНУ СЕКЦІЮ.\n");

        // --- Критична Секція ---
        // --- Операція читання значення будь-якого елемента структури ---
        printf("T2: Читаю: ID=%d, Залишок=%d, Назва='%s'\n",
               shared_product.product_id, shared_product.stock_level, shared_product.product_name);
        simulate_work(60); // Імітація читання

        // --- Операція зміни значення вказаного елемента структури ---
        printf("T2: Змінюю рівень запасу та назву...\n");
        shared_product.stock_level += 20; // Збільшуємо запас
        snprintf(shared_product.product_name, sizeof(shared_product.product_name), "Item_by_T2_%d", shared_product.stock_level);
        shared_product.product_id = 2000 + i;
        printf("T2: Змінено на: ID=%d, Залишок=%d, Назва='%s'\n",
               shared_product.product_id, shared_product.stock_level, shared_product.product_name);
        simulate_work(60); // Імітація запису

        // --- Звільнення м'ютекса (pthread_mutex_unlock) ---
        printf("T2: Звільняю м'ютекс. ВИХОДЖУ З КРИТИЧНОЇ СЕКЦІЇ.\n");
        if (pthread_mutex_unlock(&shared_data_mutex) != 0) {
            perror("T2: Помилка pthread_mutex_unlock");
            pthread_exit((void*)1);
        }
        simulate_work(120); // Робота поза критичною секцією
    }
    printf("T2: Завершив роботу.\n");
    pthread_exit(NULL);
}

// --- Основна функція main ---
int main() {
    pthread_t thread1, thread2;

    printf("Main: Початковий стан shared_product: ID=%d, Залишок=%d, Назва='%s'\n",
           shared_product.product_id, shared_product.stock_level, shared_product.product_name);

    // --- Функції керування м'ютексом: Ініціалізація ---
    // PTHREAD_MUTEX_INITIALIZER - це зручний макрос для статичної ініціалізації.
    // Для динамічної ініціалізації використовується pthread_mutex_init().
    // Ми можемо використовувати PTHREAD_MUTEX_INITIALIZER для глобальних або статичних м'ютексів.
    // Якщо м'ютекс локальний у функції або динамічно виділяється, то pthread_mutex_init.
    // В цьому випадку, оскільки м'ютекс глобальний, можна було б і не викликати init,
    // але для демонстрації явного керування:
    if (pthread_mutex_init(&shared_data_mutex, NULL) != 0) {
        perror("Main: Помилка ініціалізації м'ютекса");
        return 1;
    }
    printf("Main: М'ютекс успішно ініціалізовано.\n");

    printf("Main: Створюю потоки T1 та T2.\n");

    // Створення потоків
    if (pthread_create(&thread1, NULL, T1, NULL) != 0) {
        perror("Main: Помилка створення потоку T1");
        pthread_mutex_destroy(&shared_data_mutex); // Очистка м'ютекса
        return 1;
    }
    if (pthread_create(&thread2, NULL, T2, NULL) != 0) {
        perror("Main: Помилка створення потоку T2");
        pthread_mutex_destroy(&shared_data_mutex); // Очистка м'ютекса
        return 1;
    }

    // Очікування завершення потоків
    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);

    printf("\nMain: Обидва потоки завершили роботу.\n");
    printf("Main: Фінальний стан shared_product: ID=%d, Залишок=%d, Назва='%s'\n",
           shared_product.product_id, shared_product.stock_level, shared_product.product_name);

    // --- Функції керування м'ютексом: Знищення ---
    printf("Main: Знищую м'ютекс...\n");
    if (pthread_mutex_destroy(&shared_data_mutex) != 0) {
        perror("Main: Помилка знищення м'ютекса");
        return 1;
    }
    printf("Main: М'ютекс успішно знищено.\n");

    return 0;
}