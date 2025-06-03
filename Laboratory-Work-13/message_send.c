// message_send.c
// Програма для створення черги повідомлень System V та надсилання повідомлення.

#include <stdio.h>     // Для printf, fprintf, perror
#include <stdlib.h>    // Для EXIT_SUCCESS, EXIT_FAILURE, exit
#include <string.h>    // Для strcpy, strlen
#include <sys/types.h> // Для key_t
#include <sys/ipc.h>   // Для IPC_CREAT, IPC_EXCL (якщо потрібно)
#include <sys/msg.h>   // Для msgget, msgsnd

// Структура для повідомлення, яке буде передаватися через чергу
struct message_buf
{
    long mtype;      // Тип повідомлення (має бути > 0)
    char mtext[256]; // Текст повідомлення
};

#define LAST_GROUP_DIGIT 3        // Приклад: остання цифра номера вашої групи (m)
#define VARIANT_NUMBER 5          // Номер вашого варіанту (n)
#define SURNAME "Vyacheslavovich" // Ваше прізвище

int main()
{
    key_t msg_key;           // Ключ для ідентифікації черги повідомлень
    int msqid;               // Ідентифікатор черги повідомлень
    struct message_buf sbuf; // Буфер для відправки повідомлення
    size_t buflen;           // Довжина повідомлення

    // ---- Крок 1: Створює чергу, номер якої = (1000 + 100*m+n) ----
    msg_key = (key_t)(1000 + 100 * LAST_GROUP_DIGIT + VARIANT_NUMBER);

    printf("Спроба створити/отримати чергу повідомлень з ключем: %d\n", msg_key);

    // msgget() створює нову чергу повідомлень або повертає ID існуючої.
    // IPC_CREAT: Створити чергу, якщо її немає.
    // 0666: Права доступу (читання/запис для власника, групи, інших).
    msqid = msgget(msg_key, IPC_CREAT | 0666);
    if (msqid == -1)
    {
        perror("Помилка msgget: не вдалося створити або отримати чергу повідомлень.");
        fprintf(stderr, "Можливо, черга з цим ключем вже існує з іншими правами, або іншим користувачем.\n");
        fprintf(stderr, "Спробуйте змінити LAST_GROUP_DIGIT або VARIANT_NUMBER у коді.\n");
        return EXIT_FAILURE;
    }
    printf("Черга повідомлень успішно створена/отримана. ID черги: %d\n", msqid);

    // ---- Крок 2: Передає до черги повідомлення типу "Message from Surname" ----
    sbuf.mtype = 1;                                  // Тип повідомлення (може бути будь-яким позитивним long)
    sprintf(sbuf.mtext, "Message from %s", SURNAME); // Формуємо текст повідомлення

    buflen = strlen(sbuf.mtext) + 1; // Довжина тексту повідомлення + 1 для нульового термінатора

    printf("Надсилання повідомлення \"%s\" (розмір %zu байтів) до черги...\n", sbuf.mtext, buflen);

    // msgsnd() надсилає повідомлення до черги.
    // msqid: Ідентифікатор черги.
    // &sbuf: Вказівник на структуру повідомлення.
    // buflen: Довжина (розмір) тіла повідомлення (mtext).
    // 0: Прапори (зазвичай 0 для блокуючої операції).
    if (msgsnd(msqid, &sbuf, buflen, 0) == -1)
    {
        perror("Помилка msgsnd: не вдалося надіслати повідомлення");
        return EXIT_FAILURE;
    }

    printf("Повідомлення успішно надіслано до черги.\n");
    printf("--- Програма message_send завершила роботу ---\n");

    // Зауваження: Черга повідомлень залишається в системі після завершення цієї програми.
    // Її потрібно буде явно видалити пізніше (наприклад, за допомогою ipcrm або іншої програми).

    return EXIT_SUCCESS;
}