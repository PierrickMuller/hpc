SHELL=/bin/bash

CXX=g++
CXXFLAGS= -g -std=c++11 -Wall -pedantic -Ilib/benchmark/include/
CXXLDFLAGS_G= -Llib/benchmark/build/src/ -lbenchmark -lpthread
TXT_SIZE=10000

# for HPC server compilation
WEBFLAGS_G= -I../benchmark/include/
WEBLDFLAGS_G= -L../benchmark/build/src/

CC=gcc
CFLAGS= -O3 -fno-tree-vectorize -std=c11 -Wall -Wextra -g

BIN_G=gbench

SRC_CPP=$(wildcard *.cpp)
OBJCXX=$(SRC_CPP:%.cpp=%.o)

SRC_C=$(wildcard *.c)

OBJC=$(SRC_C:%.c=%.o)

all: $(BIN_G) generate

$(BIN_G): $(OBJC) gbench.o 
	$(CXX) -o $(BIN_G) $^ $(CXXLDFLAGS_G)

gbench.o: gbench.cpp
	$(CXX) $(CXXFLAGS) -o $@ -c $<
%.o: %.c
	$(CC) $(CFLAGS) -o $@ -c $<

generate:
	dd bs=1024 count=$(TXT_SIZE) if=/dev/urandom | tr -dc [:graph:] | fold -w 100 | head -n $(TXT_SIZE) > text.txt

verify:
	diff <(cat text.txt | tr -dc 'a-zA-Z0-9' | wc -m) <(cat result.txt | grep "count:" | head -n 1 | tr -dc '0-9\n')

clean:
	rm -f text.txt
	rm -f result.txt
	rm -f *.o
	rm -f $(BIN_G)

rung:	$(BIN_G) generate
	./$(BIN_G) > result.txt

run_web: $(BIN_G)
	./$(BIN_G) --color_print=false

run:	rung verify

.PHONY : all
.PHONY : clean
