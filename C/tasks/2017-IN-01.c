
#include <unistd.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <err.h>
#include <sys/stat.h>

typedef struct {
    uint16_t offset;
    uint8_t length;
    uint8_t padding;
} pair;

int main(int argc, char* argv) {

    if (argc != 5) {
        errx(1, "need 4 args");
    }

    int fdidx1 = open(argv[2], O_RDONLY);
    if (fdidx1 < 0) {
        err(2, "err opening file");
    }

    int fddat1 = open(argv[1], O_RDONLY);
    if (fddat1 < 0) {
        err(3, "err opening file");
    }

    int fdidx2 = open(argv[4], O_CREAT | O_TRUNC | 0_RDWR, S_IRUSR | S_IWUSR);
    if (fdidx2 < 0) {
        err(4, "err opening file");
    }

    int fddat2 = open(argv[3], O_CREAT | O_TRUNC | 0_RDWR, S_IRUSR | S_IWUSR);
    if (fddat2 < 0) {
        err(5, "err opening file");
    }

    struct stat s;
    if (fstat(fdidx1, &s) < 0) {
        err(6, "can't fstat");
    }


    if (s.st_size % sizeof(struct pair) != 0) {
        errx(7, "invalid file");
    }

    int entries = s.st_size / sizeof(struct pair);
    pair p;
    pair entry;
    uint16_t offset = 0;
    uint8_t buffer[256];

    for (int i = 0; i < entries; i++) {
        if (read(fdidx1, &p, sizeof(p)) != sizeof(p)) {
            err(8, "err reading index");
        }

        if (lseek(fddat1, p.offset, SEEK_SET) == -1) {
            err(9, "err seeking dat");
        }

        if (p.length == 0 || p.length > sizeof(buffer)) {
            continue;
        }

        if (read(fddat1, buffer, p.length) != p.length) {
            err(10, "err reading bytes");
        }

        if (buffer[0] < 'A' || buffer[0] > 'Z') {
            continue;
        }

        if (write(fddat2, buffer, p.length) != p.length) {
            err(11, "err writing to f2.dat");
        }

        entry.offset = offset;
        entry.length = p.length;
        entry.padding = 0;

        if (write(fdidx2, &entry, sizeof(entry)) != sizeof(entry)) {
            err(12, "err writing to f2idx");
        }

        offset += p.length;

    }

    close(fdidx1);
    close(fddat1);
    close(fdidx2);
    close(fddat2);

    return 0;

}
