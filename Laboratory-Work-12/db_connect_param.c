#include <stdio.h>
#include <stdlib.h>
#include <libpq-fe.h> // Необхідно для роботи з PostgreSQL

int main(int argc, char *argv[]) { // main тепер приймає аргументи командного рядка
    PGconn *conn; // Вказівник на об'єкт з'єднання з базою даних

    // Перевірка кількості аргументів командного рядка
    if (argc != 6) {
        fprintf(stderr, "Використання: %s <хост> <порт> <назва_БД> <користувач> <пароль>\n", argv[0]);
        return EXIT_FAILURE;
    }

    // Присвоєння значень з аргументів командного рядка
    char *host = argv[1];
    char *port = argv[2];
    char *db_name = argv[3];
    char *user_name = argv[4];
    char *password = argv[5];

    printf("Спроба з'єднання з базою даних PostgreSQL...\n");
    printf("Параметри з'єднання: хост=%s, порт=%s, БД=%s, користувач=%s\n",
           host, port, db_name, user_name);

    // Встановлення з'єднання з базою даних за допомогою PQsetdbLogin
    conn = PQsetdbLogin(host, port, NULL, NULL, db_name, user_name, password);

    // Перевірка статусу з'єднання
    if (PQstatus(conn) != CONNECTION_OK) {
        // Виводимо повідомлення про помилку на stderr
        fprintf(stderr, "Connect to database %s failed: %s\n",
                db_name, PQerrorMessage(conn));
        // Закриваємо з'єднання
        PQfinish(conn);
        return EXIT_FAILURE; // Повертаємо код помилки
    }

    // Якщо з'єднання успішне
    printf("Connection to database %s is successful!\n", db_name);

    // Закриття з'єднання з базою даних
    PQfinish(conn);
    return EXIT_SUCCESS; // Повертаємо код успішного виконання
}