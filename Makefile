CC = cc
CFLAGS = -ggdb -lm -Wall -Wextra

all: calc index test

test:
	$(CC) $(CFLAGS) -o test runner/test.c

index:
	$(CC) $(CFLAGS) -o index runner/index.c

calc:
	$(CC) $(CFLAGS) -o calc calc.c

clean:
	rm -f test calc index

.PHONY: calc index test
