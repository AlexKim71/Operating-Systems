// connect_plane.c

#include <stdio.h>
#include <stdlib.h>
#include <libpq-fe.h> // Для функцій з'єднання з PostgreSQL
#include "plane.h"    // Включення заголовка для власного оголошення

// Функція для встановлення з'єднання з PostgreSQL
PGconn* connect_plane(const char *host, const char *port,
                      const char *db_name, const char *user_name,
                      const char *password) {

    PGconn *conn;

    printf("Спроба з'єднання з базою даних PostgreSQL...\n");
    printf("Параметри з'єднання: хост=%s, порт=%s, БД=%s, користувач=%s\n",
           host, port, db_name, user_name);

    // Встановлення з'єднання за допомогою PQsetdbLogin
    conn = PQsetdbLogin(host, port, NULL, NULL, db_name, user_name, password);

    // Перевірка статусу з'єднання
    if (PQstatus(conn) != CONNECTION_OK) {
        fprintf(stderr, "Помилка з'єднання з базою даних %s: %s\n",
                db_name, PQerrorMessage(conn));
        // Тут не потрібно PQfinish, нехай це обробляє функція, що викликає
        return NULL; // Повертаємо NULL у випадку помилки
    }

    printf("З'єднання з базою даних %s успішне!\n", db_name);
    return conn; // Повертаємо вказівник PGconn у випадку успіху
}
