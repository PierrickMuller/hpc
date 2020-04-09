/* 09.03.2020 - Sydney Hauke - HPC - REDS - HEIG-VD */

#include <benchmark/benchmark.h>

extern "C" {
    #include "image.h"
    #include "sobel.h" 
}

const char *medalion_img = "images/medalion.png";
const char *halflife_img = "images/half-life.png";
const char *stars_img    = "images/stars.png";

#define GAUSSIAN_KERNEL_SIZE    3
#define SOBEL_KERNEL_SIZE       3

#define SOBEL_BINARY_THRESHOLD  150  // in the range 0 to uint8_max (255)

const int16_t sobel_v_kernel[SOBEL_KERNEL_SIZE*SOBEL_KERNEL_SIZE] = {
    -1, -2, -1,
     0,  0,  0,
     1,  2,  1,
};

const int16_t sobel_h_kernel[SOBEL_KERNEL_SIZE*SOBEL_KERNEL_SIZE] = {
    -1,  0,  1,
    -2,  0,  2,
    -1,  0,  1,
};

const uint16_t gauss_kernel[GAUSSIAN_KERNEL_SIZE*GAUSSIAN_KERNEL_SIZE] = {
    1, 2, 1,
    2, 4, 2,
    1, 2, 1,
};

class FiltersFixture : public benchmark::Fixture {
public:
    void SetUp(const ::benchmark::State& state)
    {
        input_img = load_image((char*)(state.range(0)));
        greyscale_img = allocate_image(input_img->width, input_img->height, COMPONENT_GRAYSCALE);
        rgb_to_grayscale(input_img, greyscale_img); 
        result_img = allocate_image(greyscale_img->width, greyscale_img->height, greyscale_img->components);
    }

    void TearDown(const ::benchmark::State&)
    {
        free_image(input_img);
        free_image(greyscale_img);
        free_image(result_img);
    }

protected:
    struct img_t *input_img;
    struct img_t *greyscale_img;
    struct img_t *result_img;
};

class RGB2GSFixture : public benchmark::Fixture {
public:
    void SetUp(const ::benchmark::State& state)
    {
        input_img = load_image((char*)(state.range(0)));
        greyscale_img = allocate_image(input_img->width, input_img->height, COMPONENT_GRAYSCALE);
    }

    void TearDown(const ::benchmark::State&)
    {
        free_image(input_img);
        free_image(greyscale_img);
    }

protected:
    struct img_t *input_img;
    struct img_t *greyscale_img;
};

BENCHMARK_DEFINE_F(RGB2GSFixture, rgb2gs_test)(benchmark::State& state)
{
    while (state.KeepRunning()) {
        rgb_to_grayscale(input_img, greyscale_img);
    }
}

BENCHMARK_DEFINE_F(FiltersFixture, gaussian_filter_test)(benchmark::State& state)
{
    while (state.KeepRunning()) {
        gaussian_filter(greyscale_img, result_img, gauss_kernel);
    }
}

BENCHMARK_DEFINE_F(FiltersFixture, sobel_filter_test)(benchmark::State& state)
{
    while (state.KeepRunning()) {
        sobel_filter(greyscale_img, result_img, sobel_v_kernel, sobel_h_kernel);
    }
}

/* RGB to GS conversion benchmarks */
BENCHMARK_REGISTER_F(RGB2GSFixture, rgb2gs_test)
    ->Arg((int64_t)medalion_img)->ArgName(medalion_img)
    ->Unit(::benchmark::kMicrosecond);
BENCHMARK_REGISTER_F(RGB2GSFixture, rgb2gs_test)
    ->Arg((int64_t)halflife_img)->ArgName(halflife_img)
    ->Unit(::benchmark::kMicrosecond);
BENCHMARK_REGISTER_F(RGB2GSFixture, rgb2gs_test)
    ->Arg((int64_t)stars_img)->ArgName(stars_img)
    ->Unit(::benchmark::kMicrosecond);

/* Gaussian filter benchmarks */
BENCHMARK_REGISTER_F(FiltersFixture, gaussian_filter_test)
    ->Arg((int64_t)medalion_img)->ArgName(medalion_img)
    ->Unit(::benchmark::kMicrosecond);
BENCHMARK_REGISTER_F(FiltersFixture, gaussian_filter_test)
    ->Arg((int64_t)halflife_img)->ArgName(halflife_img)
    ->Unit(::benchmark::kMicrosecond);
BENCHMARK_REGISTER_F(FiltersFixture, gaussian_filter_test)
    ->Arg((int64_t)stars_img)->ArgName(stars_img)
    ->Unit(::benchmark::kMicrosecond);

/* Sobel filter benchmarks */
BENCHMARK_REGISTER_F(FiltersFixture, sobel_filter_test)
    ->Arg((int64_t)medalion_img)->ArgName(medalion_img)
    ->Unit(::benchmark::kMicrosecond);
BENCHMARK_REGISTER_F(FiltersFixture, sobel_filter_test)
    ->Arg((int64_t)halflife_img)->ArgName(halflife_img)
    ->Unit(::benchmark::kMicrosecond);
BENCHMARK_REGISTER_F(FiltersFixture, sobel_filter_test)
    ->Arg((int64_t)stars_img)->ArgName(stars_img)
    ->Unit(::benchmark::kMicrosecond);

BENCHMARK_MAIN();