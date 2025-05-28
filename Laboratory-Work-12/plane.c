// plane.c

#include <stdio.h>
#include <stdlib.h>
#include <libpq-fe.h>
#include "plane.h"

int main(int argc, char *argv[]) {
    PGconn *conn = NULL;
    int result = EXIT_SUCCESS;

    if (argc != 6) {
        fprintf(stderr, "Використання: %s <хост> <порт> <назва_БД> <користувач> <пароль>\n", argv[0]);
        return EXIT_FAILURE;
    }

    const char *host = argv[1];
    const char *port = argv[2];
    const char *db_name = argv[3];
    const char *user_name = argv[4];
    const char *password = argv[5];

    conn = connect_plane(host, port, db_name, user_name, password);

    if (conn == NULL) {
        return EXIT_FAILURE;
    }

    // --- Виклик функції додавання рядка (для демонстрації, якщо потрібно) ---
    // Якщо ви хочете впевнитися, що є дані для читання,
    // додайте унікальний запис перед спробою його читання.
    // Зауваження: якщо 103 вже існує, ви отримаєте помилку.
    printf("\nВикликаємо функцію для додавання тестового запису (якщо потрібно)...\n");
    if (add_plane(conn, 103, "Airbus A380", 2005) != 0) { // Змініть a_id на унікальний
        fprintf(stderr, "Помилка додавання тестового запису (можливо, duplicate key).\n");
        // Не виходимо, можливо, ми все одно хочемо спробувати читання
    } else {
        printf("Тестовий запис успішно додано (або пропущено, якщо вже існував).\n");
    }


    // --- НОВЕ: Виклик функції перегляду рядків ---
    printf("\nВикликаємо функцію для перегляду записів...\n");
    if (get_plane(conn) != 0) {
        fprintf(stderr, "Помилка перегляду записів.\n");
        result = EXIT_FAILURE;
    } else {
        printf("Перегляд записів завершено успішно.\n");
    }

    printf("\nЗакриваємо з'єднання.\n");
    PQfinish(conn);
    return result;
}
