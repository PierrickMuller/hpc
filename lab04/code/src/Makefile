CC=gcc
CFLAGS= -O2 -ggdb -std=c11 -Wall -Wextra -pedantic -g -I../include

SRC_C=$(wildcard *.c)
OBJC=$(SRC_C:%.c=%.o)
BIN=sort

all: $(BIN)

$(BIN): $(OBJC)
	$(CC) -ggdb -o $(BIN) $^

%.o: %.c
	$(CC) $(CFLAGS) -o $@ -c $<

clean:
	rm -f *.o
	rm $(BIN)

test:
	./$(BIN) list 3000
	./$(BIN) array 3000

.PHONY : all
.PHONY : clean
.PHONY : test 
