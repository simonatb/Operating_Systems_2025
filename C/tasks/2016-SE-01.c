#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <err.h>

int compare(const void *a, const void *b) {
    return (*(unsigned char *)a - *(unsigned char *)b);
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        errx(1, "1 arg needed");
    }

    int f1 = open(argv[1], O_RDWR);
    if (f1 < 0) {
        err(2, "can't open f1");
    }

    unsigned char buffer[4096];
    ssize_t num_bytes = read(f1, buffer, sizeof(buffer));

    if (num_bytes < 0) {
        err(3, "could not read from file");
    }

    if (num_bytes > 0) {

        qsort(buffer, num_bytes, 1, compare);

        if (lseek(f1, 0, SEEK_SET) < 0) {
            err(4, "could not seek");
        }

        if (write(f1, buffer, strlen(buffer)) != num_bytes) {
            err(5, "could not write");
        }
    }

    if (close(f1) < 0) {
        err(6, "could not close file");
    }

    return 0;
}
