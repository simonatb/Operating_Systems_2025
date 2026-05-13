#include <fcntl.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <err.h>
#include <stdio.h>
#include <unistd.h>

void waitProcess(void) {
    int status;

    if (wait(&status) < 0) {
        err(6, "can't wait");
    }
    if (!WIFEXITED(status)) {
        err(7, "didnt exit");
    }
    if (WEXITSTATUS(status) != 0) {
        err(8, "didnt exit with 0");
    }
}

int main(void) {

    int pfd[2];
    if (pipe(pfd) < 0) {
        err(1, "pipe fail");
    }

    int pid1 = fork();
    if (pid1 < 0) {
        err(2, "fork failed");
    }

    if (pid1 == 0) {
        close(pfd[0]);
        dup2(pfd[1], 1);
        close(pfd[1]);

        if (execlp("cat", "cat", "/etc/passwd", (char*)NULL) < 0) {
            err(3, "child exec fail");
        }
    }
    close(pfd[1]);

    int pid2 = fork();
    if (pid2 < 0) {
        err(4, "fork 2 fail");
    }

    if (pid2 == 0) {
        dup2(pfd[0], 0);
        close(pfd[0]);

        if (execlp("grep", "grep", "s0600338", (char*)NULL) < 0) {
            err(5, "execlp 2 failed");
        }
    }

    close(pfd[0]);

    for (int i = 0; i < 2; i++) {
        waitProcess();
    }

    return 0;

}
