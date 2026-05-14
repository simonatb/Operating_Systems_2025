#include <unistd.h>
#include <err.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>

void waitProcess(void) {
    int status;

    if (wait(&status) < 0) {
        err(1, "failed wait");
    }
    if (!WIFEXITED(status)) {
        err(2, "child didnt terminate normally");
    }
    if (WEXITSTATUS(status) != 0) {
        err(3, "didnt exit with 0");
    }
}

int main(void) {

    int catfd[2];
    if (pipe(catfd) < 0) { err(4, "failed pipe"); }

    int pid1 = fork();
    if(pid1 < 0) { err(5, "failed fork"); }
    if (pid1 == 0) {
        close(catfd[0]);
        dup2(catfd[1], 1);
        close(catfd[1]);

        execlp("cat", "cat", "/etc/passwd", (char*)NULL);
        err(6, "failed cat");
    }

    close(catfd[1]);

    int grepfd[2];
    if (pipe(grepfd) < 0) { err(7, "failed pipe 2"); }

    int pid2 = fork();
    if (pid2 < 0) { err(8, "failed fork 2"); }
    if (pid2 == 0) {
        close(grepfd[0]);
        dup2(catfd[0], 0);
        close(catfd[0]);
        dup2(grepfd[1], 1);
        close(grepfd[1]);

        execlp("grep", "grep", "s0600338", (char*)NULL);
        err(9, "failed exec grep");
    }

    close(catfd[0]);
    close(grepfd[1]);

    int pid3 = fork();
    if (pid3 < 0) { err(11, "fork failed 3"); }
    if (pid3 == 0) {
        dup2(grepfd[0], 0);
        close(grepfd[0]);

        execlp("cut", "cut", "-d", ":", "-f5", (char*)NULL);
        err(12, "failed cut");
    }
    close(grepfd[0]);

    for (int i = 0; i < 3; i++) {
        waitProcess();
    }

    return 0;
}
