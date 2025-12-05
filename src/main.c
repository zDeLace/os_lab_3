#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "utils.h"
#include "mmap_append.h"

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s results_file\n", argv[0]);
        return 1;
    }

    const char *results_file = argv[1];

    int pc[2];
    int cp[2];

    if (pipe(pc) < 0 || pipe(cp) < 0) {
        perror("pipe");
        return 1;
    }

    pid_t pid = fork();
    if (pid < 0) {
        perror("fork");
        return 1;
    }

    if (pid == 0) {
        close(pc[1]);
        close(cp[0]);

        while (1) {
            int n = 0;
            ssize_t rn = read_all(pc[0], &n, sizeof(n));

            if (rn <= 0 || n == 0) break;

            float *arr = malloc(sizeof(float) * n);
            if (!arr) exit(1);

            read_all(pc[0], arr, sizeof(float) * n);

            float numerator = arr[0];
            int error = 0;

            for (int i = 1; i < n; i++) {
                if (arr[i] == 0.0f) {
                    char line[128];
                    snprintf(line, sizeof(line),
                             "Division by zero at index %d\n", i);
                    append_to_file_mmap(results_file, line);

                    int status = 1;
                    write_all(cp[1], &status, sizeof(status));
                    free(arr);
                    _exit(0);
                }

                numerator = numerator / arr[i];
            }

            char out[64];
            snprintf(out, sizeof(out), "%d\n", (int)numerator);
            append_to_file_mmap(results_file, out);

            int status = 0;
            write_all(cp[1], &status, sizeof(status));
            free(arr);
        }

        close(pc[0]);
        close(cp[1]);
        _exit(0);
    }

    else {
        close(pc[0]);
        close(cp[1]);

        char *line = NULL;
        size_t len = 0;

        printf("Enter lines of floats:\n");

        while (1) {
            printf("input> ");

            ssize_t r = getline(&line, &len, stdin);
            if (r < 0) {
                int n = 0;
                write_all(pc[1], &n, sizeof(n));
                break;
            }

            float *arr = NULL;
            int count = 0, cap = 0;

            char *save, *tok;
            tok = strtok_r(line, " \t\n", &save);

            while (tok) {
                if (count >= cap) {
                    cap = cap ? cap * 2 : 8;
                    arr = realloc(arr, cap * sizeof(float));
                }
                arr[count++] = atof(tok);
                tok = strtok_r(NULL, " \t\n", &save);
            }

            if (count == 0) {
                free(arr);
                continue;
            }

            write_all(pc[1], &count, sizeof(count));
            write_all(pc[1], arr, sizeof(float) * count);

            int status;
            read_all(cp[0], &status, sizeof(status));

            if (status == 1) {
                printf("division by zero.\n");
                free(arr);
                break;
            }

            // Локальный вывод результата
            float result = arr[0];
            for (int i = 1; i < count; i++) result /= arr[i];

            printf("Result: %d\n", (int)result);

            free(arr);
        }

        free(line);

        close(pc[1]);
        close(cp[0]);
        wait(NULL);
    }

    return 0;
}
