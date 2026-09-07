// src/verify.cpp  --  provera tacnosti u odnosu na naive
#include "matmul.hpp"
#include <vector>
#include <random>
#include <cmath>
#include <algorithm>

double max_abs_diff_vs_naive(Kernel k, int N, int bs) {
    const std::size_t n2 = static_cast<std::size_t>(N) * N;
    std::vector<double> A(n2), B(n2), C_ref(n2, 0.0), C_test(n2, 0.0);

    std::mt19937 rng(1234);
    std::uniform_real_distribution<double> d(-1.0, 1.0);
    for (std::size_t i = 0; i < n2; ++i) { A[i] = d(rng); B[i] = d(rng); }

    matmul_naive(A.data(), B.data(), C_ref.data(),  N, bs);
    k          (A.data(), B.data(), C_test.data(), N, bs);

    double m = 0.0;
    for (std::size_t i = 0; i < n2; ++i)
        m = std::max(m, std::fabs(C_ref[i] - C_test[i]));
    return m;
}

bool verify_against_naive(Kernel k, int N, int bs, double tol) {
    return max_abs_diff_vs_naive(k, N, bs) <= tol;
}
