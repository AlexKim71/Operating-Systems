// get_plane.c

#include <stdio.h>
#include <stdlib.h>
#include <libpq-fe.h> // Для роботи з PostgreSQL
#include "plane.h"    // Для оголошення функції connect_plane та PGconn

// Функція для перегляду рядків з таблиці 'plane'
// Приймає вказівник на PGconn.
// Повертає 0 у випадку успіху, 1 у випадку помилки.
int get_plane(PGconn *conn)
{
    PGresult *res; // Для зберігання результату виконання SQL-команд
    int num_rows;
    int i, j;

    if (conn == NULL)
    {
        fprintf(stderr, "Помилка: З'єднання з БД не встановлено (conn == NULL).\n");
        return 1; // Повертаємо помилку
    }

    printf("\n--- Початок транзакції для перегляду записів у 'plane' ---\n");

    // 1. Початок транзакції
    res = PQexec(conn, "START TRANSACTION");
    if (PQresultStatus(res) != PGRES_COMMAND_OK)
    {
        fprintf(stderr, "Помилка START TRANSACTION: %s", PQerrorMessage(conn));
        PQclear(res);
        return 1;
    }
    printf("Команда 'START TRANSACTION' виконана успішно.\n");
    PQclear(res);

    // 2. Блокування таблиці (рекомендовано для консистентності, хоча для SELECT часто використовують SHARED MODE)
    // Завдання просить LOCK TABLE, тому використовуємо його.
    res = PQexec(conn, "LOCK TABLE plane IN SHARE MODE"); // SHARE MODE для читання, щоб не блокувати запис
    if (PQresultStatus(res) != PGRES_COMMAND_OK)
    {
        fprintf(stderr, "Помилка LOCK TABLE plane: %s", PQerrorMessage(conn));
        PQexec(conn, "ROLLBACK");
        PQclear(res);
        return 1;
    }
    printf("Команда 'LOCK TABLE plane' виконана успішно.\n");
    PQclear(res);

    // 3. Виконання запиту SELECT
    printf("Виконання команди SELECT: SELECT a_id, model, year FROM plane ORDER BY a_id\n");
    res = PQexec(conn, "SELECT a_id, model, year FROM plane ORDER BY a_id");

    if (PQresultStatus(res) != PGRES_TUPLES_OK)
    {
        fprintf(stderr, "Помилка SELECT FROM plane: %s", PQerrorMessage(conn));
        PQexec(conn, "ROLLBACK");
        PQclear(res);
        return 1;
    }
    printf("Команда 'SELECT FROM plane' виконана успішно.\n");

    // Отримання та виведення результатів
    num_rows = PQntuples(res);
    int num_cols = PQnfields(res);

    if (num_rows == 0)
    {
        printf("Таблиця 'plane' не містить записів.\n");
    }
    else
    {
        printf("\n--- Результати запиту SELECT ---\n");
        // Виведення заголовків стовпців
        for (j = 0; j < num_cols; j++)
        {
            printf("%-15s", PQfname(res, j)); // PQfname повертає назву стовпця
        }
        printf("\n");
        for (j = 0; j < num_cols; j++)
        {
            printf("---------------"); // Розділювач
        }
        printf("\n");

        // Виведення даних
        for (i = 0; i < num_rows; i++)
        {
            for (j = 0; j < num_cols; j++)
            {
                printf("%-15s", PQgetvalue(res, i, j)); // PQgetvalue повертає значення поля
            }
            printf("\n");
        }
        printf("--------------------------------\n");
    }

    PQclear(res); // Очищаємо результат запиту

    // 4. Завершення транзакції (COMMIT)
    res = PQexec(conn, "COMMIT");
    if (PQresultStatus(res) != PGRES_COMMAND_OK)
    {
        fprintf(stderr, "Помилка COMMIT: %s", PQerrorMessage(conn));
        PQclear(res);
        return 1;
    }
    printf("Команда 'COMMIT' виконана успішно. Транзакцію завершено.\n");
    PQclear(res);

    printf("--- Перегляд записів завершено. ---\n");
    return 0; // Успіх
}