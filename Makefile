CC=gcc
CFLAGS=-Wall -Wextra -std=c11 -pedantic -g
SRC=main.c

all: allocation_optmizer

allocation_optmizer: main.c
	$(CC) $(CFLAGS) -o allocation_optmizer $(SRC)

clean:
	rm allocation_optmizer

run: allocation_optmizer
	./allocation_optmizer entradaProj2TAG.txt

debug: allocation_optmizer
	gdb --args ./allocation_optmizer entradaProj2TAG.txt
