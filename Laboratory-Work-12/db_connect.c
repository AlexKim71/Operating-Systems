#include <stdio.h>
#include <stdlib.h>
#include <libpq-fe.h>

int main(void)
{
    PGconn *conn;

    char *host = "localhost";
    char *port = "5432";
    char *db_name = "ai243-vyacheslavovich_db";
    char *user_name = "ai243-vyacheslavovich";
    char *password = "0109206kim";

    printf("Спроба з'єднання з базою даних PostgreSQL...\n");
    printf("Параметри з'єднання: хост=%s, порт=%s, БД=%s, користувач=%s\n",
           host, port, db_name, user_name);

    conn = PQsetdbLogin(host, port, NULL, NULL, db_name, user_name, password);

    if (PQstatus(conn) != CONNECTION_OK)
    {
        fprintf(stderr, "Connect to database %s failed: %s\n",
                db_name, PQerrorMessage(conn));
        PQfinish(conn);
        return EXIT_FAILURE;
    }

    printf("Connection to database %s is successful!\n", db_name);

    PQfinish(conn);
    return EXIT_SUCCESS;
}