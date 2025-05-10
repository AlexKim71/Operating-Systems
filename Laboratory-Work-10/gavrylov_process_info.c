#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>

int main() {
    pid_t pgid = getpgrp();
    printf("Ідентифікатор групи процесів (PGID): %d\n", pgid);

    pid_t pid = getpid();
    printf("Ідентифікатор процесу (PID): %d\n", pid);

    pid_t ppid = getppid();
    printf("Ідентифікатор parent-процесу (PPID): %d\n", ppid);

    uid_t uid = getuid();
    printf("Ідентифікатор користувача процесу (UID): %d\n", uid);

    gid_t gid = getgid();
    printf("Ідентифікатор групи користувача процесу (GID): %d\n", gid);

    return 0;
}