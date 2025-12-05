#define _GNU_SOURCE
#include "mmap_append.h"
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>

int append_to_file_mmap(const char *filename, const char *text) {
    int fd = open(filename, O_RDWR | O_CREAT, 0644);
    if (fd < 0) {
        perror("open");
        return -1;
    }

    struct stat st;
    if (fstat(fd, &st) < 0) {
        perror("fstat");
        close(fd);
        return -1;
    }

    off_t old_size = st.st_size;
    size_t add = strlen(text);
    off_t new_size = old_size + add;

    if (ftruncate(fd, new_size) < 0) {
        perror("ftruncate");
        close(fd);
        return -1;
    }

    void *map = mmap(NULL, new_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (map == MAP_FAILED) {
        perror("mmap");
        close(fd);
        return -1;
    }

    memcpy((char*)map + old_size, text, add);

    if (msync(map, new_size, MS_SYNC) < 0)
        perror("msync");

    munmap(map, new_size);
    close(fd);
    return 0;
}
