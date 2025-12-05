#define _GNU_SOURCE
#ifndef MMAP_APPEND_H
#define MMAP_APPEND_H

int append_to_file_mmap(const char *filename, const char *text);

#endif
