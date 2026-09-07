// include/matmul.hpp
#pragma once
#include <cstddef>
#include <functional>

// Potpis svakog kernela: (A, B, C, N, block_size)
// block_size se ignorise tamo gde nije relevantan.
using Kernel = void (*)(const double*, const double*, double*, int, int);

void matmul_naive   (const double* A, const double* B, double* C, int N, int bs);
void matmul_ikj     (const double* A, const double* B, double* C, int N, int bs);
void matmul_tiled   (const double* A, const double* B, double* C, int N, int bs);
void matmul_parallel(const double* A, const double* B, double* C, int N, int bs);
void matmul_blas    (const double* A, const double* B, double* C, int N, int bs);
void matmul_ijk_transposed(const double* A, const double* B, double* C, int N, int bs);

struct TimingResult {
    double median_ms;
    double min_ms;
    double max_ms;
    int    reps;
};

TimingResult time_kernel(const std::function<void()>& f, int warmup = 3, int reps = 7);
double gflops(int N, double ms);

// Vraca max apsolutnu razliku u odnosu na naive (manje je bolje).
double max_abs_diff_vs_naive(Kernel k, int N, int bs);
bool   verify_against_naive (Kernel k, int N, int bs, double tol = 1e-9);

double measure_bandwidth_gbps(std::size_t n_elems);
