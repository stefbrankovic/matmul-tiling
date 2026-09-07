// src/bandwidth.cpp  --  STREAM triad: a[i] = b[i] + s*c[i]
// Daje izmerenu propusnost memorije, potrebnu za kosu liniju roofline dijagrama.
#include "matmul.hpp"
#include <vector>
#include <chrono>

double measure_bandwidth_gbps(std::size_t n) {
    std::vector<double> a(n, 1.0), b(n, 2.0), c(n, 3.0);
    const double s = 3.0;

    for (std::size_t i = 0; i < n; ++i) a[i] = b[i] + s * c[i];   // warmup

    auto t0 = std::chrono::steady_clock::now();
    for (std::size_t i = 0; i < n; ++i) a[i] = b[i] + s * c[i];
    auto t1 = std::chrono::steady_clock::now();

    // sink: sprecava kompajler da izbaci ceo racun kao mrtav kod
    static volatile double sink;
    sink = a[n / 2];
    (void)sink;

    double sec   = std::chrono::duration<double>(t1 - t0).count();
    double bytes = 3.0 * static_cast<double>(n) * sizeof(double);  // 2 citanja + 1 upis
    return bytes / sec / 1e9;
}
