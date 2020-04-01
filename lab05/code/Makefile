CC=gcc
CXX=g++

SOBEL_CFLAGS= -O3 -g -Wall -I../../lib/stb/ -fno-inline
BENCHMARK_CXXFLAGS= -O3 -g -Wall -I../../lib/stb -I../../lib/benchmark/include -DNDEBUG
SOBEL_LDFLAGS= -lm 
BENCHMARK_LDFLAGS= -L../../lib/benchmark/build/src -lbenchmark -lpthread -lm

SOBEL=sobel
BENCHMARK=sobel-bm 

OBJECTS = $(patsubst %.c, %.o, $(wildcard *.c))
CPP_OBJECTS = $(patsubst %.cpp, %.o, $(wildcard *.cpp))
HEADERS = $(wildcard *.h)

.PHONY: default all clean 

default: all

all: $(SOBEL) $(BENCHMARK)

$(SOBEL): $(OBJECTS)
	$(CC) $(OBJECTS) $(SOBEL_LDFLAGS) -o $@ 

$(BENCHMARK): $(CPP_OBJECTS) sobel.o image.o
	$(CXX) $^ $(BENCHMARK_LDFLAGS) -o $@ 

%.o: %.c 
	$(CC) -c $^ $(SOBEL_CFLAGS) -o $@ 

%.o: %.cpp
	$(CXX) -c $^ $(BENCHMARK_CXXFLAGS) -o $@ 

clean:
	rm -f $(BENCHMARK)
	rm -f $(SOBEL)
	rm -f $(OBJECTS)
	rm -f $(CPP_OBJECTS)
