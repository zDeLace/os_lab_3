#define _GNU_SOURCE
#include "utils.h"
#include <errno.h>

ssize_t write_all(int fd, const void *buf, size_t count) {
    const char *p = buf;
    size_t written = 0;

    while (written < count) {
        ssize_t w = write(fd, p + written, count - written);
        if (w < 0) {
            if (errno == EINTR) continue;
            return -1;
        }
        written += w;
    }
    return written;
}

ssize_t read_all(int fd, void *buf, size_t count) {
    char *p = buf;
    size_t read_bytes = 0;

    while (read_bytes < count) {
        ssize_t r = read(fd, p + read_bytes, count - read_bytes);
        if (r < 0) {
            if (errno == EINTR) continue;
            return -1;
        }
        if (r == 0) break;
        read_bytes += r;
    }
    return read_bytes;
}
