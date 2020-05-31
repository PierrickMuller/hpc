CC=gcc
CXX=g++

IFS_CFLAGS= -O3 -g -Wall -Ilib/stb/ -fno-inline
BENCHMARK_CXXFLAGS= -O3 -g -Wall -Ilib/stb -Ilib/benchmark/include -DNDEBUG
IFS_LDFLAGS= -lm 
BENCHMARK_LDFLAGS= -Llib/benchmark/build/src -lbenchmark -lpthread -lm

IFS=ifs
BENCHMARK=ifs-bm 

OBJECTS = $(patsubst %.c, %.o, $(wildcard *.c))
CPP_OBJECTS = $(patsubst %.cpp, %.o, $(wildcard *.cpp))
HEADERS = $(wildcard *.h)

.PHONY: default all clean 

default: all

all: $(IFS) $(BENCHMARK)

$(IFS): $(OBJECTS)
	$(CC) $(OBJECTS) $(IFS_LDFLAGS) -o $@ 

$(BENCHMARK): $(CPP_OBJECTS) image.o random.o
	$(CXX) $^ $(BENCHMARK_LDFLAGS) -o $@ 

%.o: %.c 
	$(CC) -c $^ $(IFS_CFLAGS) -o $@ 

%.o: %.cpp
	$(CXX) -c $^ $(BENCHMARK_CXXFLAGS) -o $@ 

clean:
	rm -f $(BENCHMARK)
	rm -f $(IFS)
	rm -f $(OBJECTS)
	rm -f $(CPP_OBJECTS)
