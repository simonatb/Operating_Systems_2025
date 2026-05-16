#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <fcntl.h>

int main(int argc, char* argv[]) {
    if (argc < 2) {
        errx(26, "need one arg at least");
    }

    int pfd[2];
    if (pipe(pfd) < 0) {
        err(26, "failed pipe");
    }

    for (int i = 1; i < argc; i++) {
        int pid = fork();
        if (pid < 0) {
            err(26, "failed fork");
        }

        if (pid == 0) {
            close(pfd[0]);
            if (dup2(pfd[1], 1) < 0) {
                err(26, "failed dup2");
            }
            close(pfd[1]);

            execlp(argv[i], argv[i], (const char*) NULL);
            err(26, "failed exec %s", argv[i]);
        }
    }

    close(pfd[1]);

    char byte;
    size_t curr = 0;
    while (read(pfd[0], &byte, sizeof(char)) == sizeof(char)) {
        if (byte == str[cur]) {
            curr++;
        } else {
            curr = 0;
        }

        if (curr == strlen(str)) {
            close(pfd[0]);
            if (kill(0, SIGTERM) < 0) {
                err(26, "failed to kill all children");
            }
            exit (0);
        }
    }
    close(pfd[0]);

    exit(1);
}
