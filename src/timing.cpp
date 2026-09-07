// src/timing.cpp
#include "matmul.hpp"
#include <chrono>
#include <vector>
#include <algorithm>

TimingResult time_kernel(const std::function<void()>& f, int warmup, int reps) {
    for (int i = 0; i < warmup; ++i) f();

    std::vector<double> t;
    t.reserve(reps);
    for (int i = 0; i < reps; ++i) {
        auto s = std::chrono::steady_clock::now();
        f();
        auto e = std::chrono::steady_clock::now();
        t.push_back(std::chrono::duration<double, std::milli>(e - s).count());
    }
    std::sort(t.begin(), t.end());
    return { t[t.size() / 2], t.front(), t.back(), reps };
}

double gflops(int N, double ms) {
    return (2.0 * N * N * N) / (ms * 1e6);   // 2*N^3 FLOP
}
