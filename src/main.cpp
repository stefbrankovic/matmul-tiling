// src/main.cpp
// CLI:  ./matmul <N> <verzija> [block_size] [opcije]
//   verzije : naive | ikj | tiled | parallel | blas | bandwidth
//   opcije  : --csv          red za results.csv
//             --csv-sweep    red za sweep.csv
//             --verify       poredi sa naive i ispisi max apsolutnu razliku
//             --single       jedan prolaz bez merenja (za `perf stat`)
//             --reps N       broj merenih prolaza (default 7)
//             --warmup N     broj warmup prolaza (default 3)
#include "matmul.hpp"

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <random>
#include <vector>

static Kernel pick(const std::string& v) {
    if (v == "naive")    return matmul_naive;
    if (v == "ikj")      return matmul_ikj;
    if (v == "tiled")    return matmul_tiled;
    if (v == "parallel") return matmul_parallel;
    if (v == "blas")     return matmul_blas;
    if (v == "ijkT")     return matmul_ijk_transposed;
    return nullptr;
}

int main(int argc, char** argv) {
    if (argc < 3) {
        std::fprintf(stderr,
            "usage: %s <N> <naive|ikj|ijkT|tiled|parallel|blas|bandwidth> [bs] "
            "[--csv|--csv-sweep|--verify|--single] [--reps N] [--warmup N]\n", argv[0]);
        return 1;
    }

    const int   N = std::atoi(argv[1]);
    const std::string version = argv[2];

    int  bs        = 32;
    bool csv       = false;
    bool csv_sweep = false;
    bool do_verify = false;
    bool single    = false;
    int  reps      = -1;      // -1 = automatski
    int  warmup    = -1;

    for (int i = 3; i < argc; ++i) {
        std::string a = argv[i];
        if      (a == "--csv")       csv = true;
        else if (a == "--csv-sweep") csv_sweep = true;
        else if (a == "--verify")    do_verify = true;
        else if (a == "--single")    single = true;
        else if (a == "--reps"   && i + 1 < argc) reps   = std::atoi(argv[++i]);
        else if (a == "--warmup" && i + 1 < argc) warmup = std::atoi(argv[++i]);
        else if (a.size() && a[0] != '-')         bs     = std::atoi(a.c_str());
    }

    if (version == "bandwidth") {
        std::size_t n = (N > 0) ? static_cast<std::size_t>(N) * 1000000ULL : 50000000ULL;
        std::printf("STREAM-triad bandwidth: %.2f GB/s  (n=%zu elems, %.1f MB/array)\n",
                    measure_bandwidth_gbps(n), n, n * 8.0 / 1e6);
        return 0;
    }

    Kernel k = pick(version);
    if (!k) { std::fprintf(stderr, "nepoznata verzija: %s\n", version.c_str()); return 1; }
    if (N <= 0) { std::fprintf(stderr, "N mora biti > 0\n"); return 1; }

    if (do_verify) {
        double diff = max_abs_diff_vs_naive(k, N, bs);
        std::printf("verify %-8s N=%-5d bs=%-4d  max|diff| = %.3e  -> %s\n",
                    version.c_str(), N, bs, diff, diff <= 1e-9 ? "OK" : "FAIL");
        return diff <= 1e-9 ? 0 : 3;
    }

    // Naive je O(N^3) sa uzasnom konstantom: za velike N smanji broj prolaza,
    // inace ceo run_all.sh traje pola sata.
    if (reps   < 0) reps   = (version == "naive" && N >= 1024) || N >= 2048 ? 3 : 7;
    if (warmup < 0) warmup = (version == "naive" && N >= 1024) || N >= 2048 ? 1 : 3;

    const std::size_t n2 = static_cast<std::size_t>(N) * N;
    std::vector<double> A(n2), B(n2), C(n2, 0.0);
    std::mt19937 rng(42);
    std::uniform_real_distribution<double> d(-1.0, 1.0);
    for (std::size_t i = 0; i < n2; ++i) { A[i] = d(rng); B[i] = d(rng); }

    if (single) {                       // za perf stat: jedan prolaz, bez warmupa
        k(A.data(), B.data(), C.data(), N, bs);
        double checksum = 0.0;
        for (std::size_t i = 0; i < n2; ++i) checksum += C[i];
        std::fprintf(stderr, "checksum=%.6e\n", checksum);
        return 0;
    }

    TimingResult r = time_kernel(
        [&] { k(A.data(), B.data(), C.data(), N, bs); }, warmup, reps);
    const double g = gflops(N, r.median_ms);

    // Checksum: bez njega kompajler sme da izbaci ceo racun kao mrtav kod.
    double checksum = 0.0;
    for (std::size_t i = 0; i < n2; ++i) checksum += C[i];

    // Prazan TODO kernel vraca same nule -> apsurdno visok GFLOPS.
    // Bez ovog upozorenja se lako izgubi pola sata na "neverovatan rezultat".
    if (checksum == 0.0 && version != "naive")
        std::fprintf(stderr,
            "UPOZORENJE: checksum je tacno 0 -- kernel '%s' verovatno jos nije "
            "implementiran (prazan TODO). Izmereni GFLOPS je besmislen.\n",
            version.c_str());

    if (csv) {
        std::printf("%d,%s,%d,%.3f,%.3f,%.3f,%.4f\n",
                    N, version.c_str(), bs, r.median_ms, r.min_ms, r.max_ms, g);
        std::fprintf(stderr, "checksum=%.6e\n", checksum);
    } else if (csv_sweep) {
        std::printf("%d,%d,%.3f,%.4f\n", N, bs, r.median_ms, g);
        std::fprintf(stderr, "checksum=%.6e\n", checksum);
    } else {
        std::printf("%-8s N=%-5d bs=%-4d  median %9.3f ms  (min %9.3f, max %9.3f, reps %d)"
                    "  %7.3f GFLOPS  checksum=%.6e\n",
                    version.c_str(), N, bs, r.median_ms, r.min_ms, r.max_ms, r.reps, g, checksum);
    }
    return 0;
}
