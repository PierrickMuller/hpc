/* 26.01.16 - Xavier Ruppen - HPC - REDS - HEIG-VD */

#include <stdio.h>
#include <stdlib.h>
#include "benchmark/benchmark.h"
#include "bench.h"

extern "C" {
#include "stats_count.h"
}

class CharFixture : public benchmark::Fixture {
public :
    void SetUp(const ::benchmark::State&)
    {
        stats = stats_init(FILENAME);
    }

    void TearDown(const ::benchmark::State&)
    {
        stats_clear(stats);
        printf("count: %ld\n", stats->count);
    }

protected :
    struct char_stats *stats;
};


BENCHMARK_F(CharFixture, stats_count)(benchmark::State& state) {
    while (state.KeepRunning()) {
      stats->count = stats_count(stats);
    }
}

BENCHMARK_MAIN();
