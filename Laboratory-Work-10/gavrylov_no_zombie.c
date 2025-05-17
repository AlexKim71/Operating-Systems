#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>

// Signal handler for SIGCHLD
void sighandler(int sig) {
    printf("Signal handler: received SIGCHLD (signal %d)\n", sig);
    // Reap the terminated child process
    wait(NULL);
}

int main(void) {
    // Number of seconds to wait
    int n = 16;
    int wait_time = 3 * n; // 48 seconds in this case

    // Register the signal handler for SIGCHLD
    if (signal(SIGCHLD, sighandler) == SIG_ERR) {
        perror("Failed to register signal handler");
        exit(EXIT_FAILURE);
    }

    // Fork the process
    pid_t pid = fork();

    if (pid == -1) {
        perror("Fork failed");
        exit(EXIT_FAILURE);
    }
    else if (pid == 0) {
        // Child process
        printf("Child process (PID: %d) is running.\n", getpid());
        // Simulate some work (optional)
        // sleep(1);
        printf("Child process (PID: %d) is finished.\n", getpid());
        // Exit without flushing buffers
        _exit(0);
    }
    else {
        // Parent process
        printf("Parent process (PID: %d) started. Child PID: %d\n", getpid(), pid);
        printf("Parent process is waiting for %d seconds...\n", wait_time);
        // Wait for the specified time
        sleep(wait_time);
        printf("Parent process finished.\n");
    }

    return EXIT_SUCCESS;
}
