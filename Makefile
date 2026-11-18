CC = cc
CFLAGS = -ggdb -lm -Wall -Wextra

all: calc.c  index.c main.c

calc:
	$(CC) $(CFLAGS) -o calc test/calc.c

index:
	$(CC) $(CFLAGS) -o index test/index.c

main:
	$(CC) $(CFLAGS) -o main main.c

clean:
	rm -f *.o *.out main calc index
