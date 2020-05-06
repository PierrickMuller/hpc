/* 06.05.2020 - Sydney Hauke - HPC - REDS - HEIG-VD */

#include <benchmark/benchmark.h>

extern "C" {
    #include "image.h"
    #include "random.h"
    #include "ifs.c"
}

#define PASSES      150 
#define MIN_WIDTH   1000

class IFSFixture : public benchmark::Fixture {
public:
    void SetUp(const ::benchmark::State& state)
    {
        width = MIN_WIDTH;
        height = width * (bf_ifs.height / bf_ifs.width);

        res = allocate_image(width, height, COMPONENT_GRAYSCALE);
    }

    void TearDown(const ::benchmark::State&)
    {
        free_image(res);
    }

protected:
    struct img_t *res;
    size_t width, height;
};

BENCHMARK_DEFINE_F(IFSFixture, ifs_test)(benchmark::State& state)
{
    size_t count_points = 0;
    while (state.KeepRunning()) {
        count_points = __ifs(res, &bf_ifs, PASSES, width, height);
    }

    state.counters["points/s"] = benchmark::Counter(count_points, benchmark::Counter::kIsRate);
    state.counters["total_points"] = count_points;
}

BENCHMARK_REGISTER_F(IFSFixture, ifs_test)
    ->Unit(::benchmark::kMicrosecond);

BENCHMARK_MAIN();
