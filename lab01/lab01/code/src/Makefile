CXX=g++
CXXFLAGS= -g -std=c++11 -Wall -pedantic -I../../../../lib/benchmark/include/
CXXLDFLAGS_G= -L../../lib/benchmark/build/src/ -lbenchmark -lpthread

CC=gcc
CFLAGS= -O3 -std=c11 -Wall -Wextra -pedantic -Werror -g

BIN_G=gbench

SRC_CPP=$(wildcard *.cpp)
OBJCXX=$(SRC_CPP:%.cpp=%.o)

SRC_C=$(wildcard *.c)

OBJC=$(SRC_C:%.c=%.o)

all: $(BIN_G)

$(BIN_G): $(OBJC) gbench.o
	$(CXX) -o $(BIN_G) $^ $(CXXLDFLAGS_G)

gbench.o: gbench.cpp
	$(CXX) $(CXXFLAGS) -o $@ -c $<
%.o: %.c
	$(CC) $(CFLAGS) -o $@ -c $<

clean:
	rm -f *.o
	rm $(BIN_G)

rung:	$(BIN_G)
	./$(BIN_G)

run:	rung

.PHONY : all
.PHONY : clean
