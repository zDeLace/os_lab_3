CC=gcc
CFLAGS=-Wall -Wextra -std=c11 -Iinclude
OBJ=src/main.o src/utils.o src/mmap_append.o

all: lab3

lab3: $(OBJ)
	$(CC) $(CFLAGS) -o lab3 $(OBJ)

src/main.o: src/main.c include/utils.h include/mmap_append.h
src/utils.o: src/utils.c include/utils.h
src/mmap_append.o: src/mmap_append.c include/mmap_append.h

clean:
	rm -f src/*.o lab3
