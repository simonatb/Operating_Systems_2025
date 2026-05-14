#include <unistd.h>
#include <sys/wait.h>
#include <err.h>
#include <fcntl.h>

typedef struct {
    char filename[8];
    uint32_t offset;
    uint32_t length;
} triple;

void waitProcess(void) {
    int status;
    if (wait(&status) < 0) {
        err(17, "cant wait");
    }
    if (!WIFEXITED(status)) {
        err(18, "didnt exit");
    }
    if (WEXITSTATUS(status) != 0) {
        err(19, "didnt exit with 0");
    }
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        errx(1, "need 1 arg file");
    }

    int fd = open(argv[1], O_RDONLY);
    if (fd < 0) {
        err(2, "failed open");
    }

    struct stat s;

    if (fstat(fd, &s) < 0) {
        err(3, "fstat failed");
    }

    triple t;
    if (s.st_size % sizeof(t) != 0) {
        errx(4, "wrong format");
    }

    int pipefd[2];
    if (pipe(pipefd) < 0) {
        err(5, "failed pipe");
    }

    int size = s.st_size / sizeof(t);
    if (size > 8) {
        errx(6, "more than 8 triples");
    }

    uint16_t res = 0;
    int readbytes;

    for (int i = 0; i < size; i++) {
        if ((readbytes = read(fd, &c, sizeof(c))) != sizeof(c)) {
            err(7, "err reading");
        }

        pid_t pid = fork();
        if (pid < 0) {
            err(8, "failed fork");
        }

        if (pid == 0) {
            close(pipefd[0]);

            int filefd = open(c.filename, O_RDONLY);
            if (filefd < 0) {
                err(9, "err open in child file");
            }

            struct stat f;
            if (fstat(filefd, &f) < 0) {
                err(10, "failed fstat in child");
            }

            if (f.st_size/sizeof(uint16_t) < t.offset || f.st_size/sizeof(uint16_t) < t.offset + t.length) {
                errx(11, "incorrect format");
            }

            if (lseek(filefd, t.offset * sizeof(uint16_t), SEEK_SET) < 0) {
                err(12, "lseek fail");
            }

            uint16_t result = 0;
            uint16_t toxor;
            for (int j = 0; j < t.length; j++) {
                if ((readbytes=read(filefd, &toxor, sizeof(uint16_t))) != sizeof(uint16_t)) {
                    err(13, "failed read bytes");
                }
                result^=toxor;
            }

            close(filefd);

            if (write(pipefd[1], &result, sizeof(result)) < sizeof(result)) {
                err(14, "err writing to pipe from child");
            }

            close(pipefd[1]);
            exit(0);
        }
    }

    close(pipefd[0]);
    close(fd);

    char res_str[8];
    snprintf(res_str, sizeof(res_str), "%04X\n", res);
    if (write(1, res_str, sizeof(res_str)) < 0) {
        err(15, "err writing res to stdout");
    }

    for (int i = 0; i < size; i++) {
        waitProcess();
    }

    return 0;
}
