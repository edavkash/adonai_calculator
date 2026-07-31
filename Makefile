CC = cc
CFLAGS = -ggdb -lm -Wall -Wextra

all: calc.o # index.o

calc:
	$(CC) $(CFLAGS) -o $@.out calc.o
# index:
# 	$(CC) $(CFLAGS) -o $@.out index.o

clean:
	rm -f *.o *.out
