/* 26.01.16 - Xavier Ruppen - HPC - REDS - HEIG-VD */
/* 23.03.20 - Modification par Pierrick Muller pour le laboratoire 3 d'HPC */

#include <stdio.h>
#include <stdlib.h>
#include "benchmark/benchmark.h"
#include "bench.h"

#define DATA_LEN 100000

extern "C" {
#include "array_util.h"
#include "list_util.h"
}

class SortFixtureArray : public benchmark::Fixture {
public :
    void SetUp(const ::benchmark::State&)
    {
      array_uint64 = array_init(DATA_LEN);

    }

    void TearDown(const ::benchmark::State&)
    {
      array_clear(array_uint64);
    }

protected :
  uint64_t *array_uint64;
};

class SortFixtureList : public benchmark::Fixture {
public :
    struct list_element *head;

    void SetUp(const ::benchmark::State&)
    {

    }

    void TearDown(const ::benchmark::State&)
    {

    }

};


BENCHMARK_F(SortFixtureArray, array_sort)(benchmark::State& state) {
    while (state.KeepRunning()) {

      array_sort(array_uint64,DATA_LEN);
    }
}

BENCHMARK_F(SortFixtureList, list_sort)(benchmark::State& state) {
    while (state.KeepRunning()) {
      // Je laisse l'init et le clear ici, car je suis sure que leur temps de fonctionnement est pris en compte dans le benchmark
      head = list_init(DATA_LEN);
      list_sort(head);
      list_clear(head);
    }
}

BENCHMARK_MAIN();
