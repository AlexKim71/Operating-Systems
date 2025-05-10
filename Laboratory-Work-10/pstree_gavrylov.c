#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main() {
    pid_t pid;
    char *args[] = {"pstree", "-u", "-s", NULL}; // Команда та її аргументи
    char gavrylov[] = "gavrylov";

    pid = fork();

    if (pid == -1) {
        perror("Помилка fork");
        exit(EXIT_FAILURE);
    } else if (pid == 0) {
        // Код для child-процесу
        printf("the child of %s executes: pstree -u -s\n", gavrylov);
        sleep(1); // Затримка, щоб повідомлення не з'явилося після execv (за рекомендацією)

        // Заміна образу процесу
        execvp(args[0], args);

        // Якщо execvp повертає значення, сталася помилка
        perror("Помилка execvp");
        exit(EXIT_FAILURE);
    } else {
        // Код для parent-процесу
        wait(NULL); // Очікування завершення child-процесу
        printf("Child-процес завершив виконання.\n");
    }

    return 0;
}