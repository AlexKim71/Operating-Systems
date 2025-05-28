// plane.h

#ifndef PLANE_H
#define PLANE_H

#include <libpq-fe.h>

// Функція для встановлення з'єднання з PostgreSQL
PGconn* connect_plane(const char *host, const char *port,
                      const char *db_name, const char *user_name,
                      const char *password);

// Оголошення функції для додавання рядка в таблицю 'plane'
int add_plane(PGconn *conn, int a_id, const char *model, int year);

// НОВЕ: Оголошення функції для перегляду рядків з таблиці 'plane'
// Повертає 0 у випадку успіху, 1 у випадку помилки.
int get_plane(PGconn *conn);

#endif // PLANE_H
