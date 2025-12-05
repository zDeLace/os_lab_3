#define _GNU_SOURCE
#ifndef UTILS_H
#define UTILS_H

#include <unistd.h>
#include <stddef.h>

ssize_t write_all(int fd, const void *buf, size_t count);
ssize_t read_all(int fd, void *buf, size_t count);

#endif
