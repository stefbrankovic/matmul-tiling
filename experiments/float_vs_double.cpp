// experiments/float_vs_double.cpp
//
// EKSPERIMENT 5 (opciono, utorak) -- zasto tip podatka menja optimalni blok.
//
//   g++ -std=c++17 -O2 -o /tmp/fvd experiments/float_vs_double.cpp
//   /tmp/fvd 1024
//
// PRE POKRETANJA upisi predvidjanje u docs/notes.md:
//   double: 8 elemenata po cache liniji od 64 B, uslov 3*BS^2*8 <= L1
//   float:  16 elemenata po liniji,             uslov 3*BS^2*4 <= L1
//   -> BS_float / BS_double = sqrt(2) ~= 1.41
//   Za L1=32 KB:  BS_double <= 36,  BS_float <= 52
//
// Pitanje na koje moras da odgovoris posle merenja: da li se odnos poklopio
// sa sqrt(2), i ako nije -- da li je krivac asocijativnost keša, rezija
// petlji, ili auto-vektorizacija (float staje 2x vise u SIMD registar)?
//
// TODO: telo kernela napisi sam -- to je tvoja tiled implementacija,
//       samo sablonizovana po tipu T. Ako je vec imas, prepisivanje je
//       mehanicko i traje dva minuta.

#include <algorithm>
#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <random>
#include <vector>

template <typename T>
void tiled(const T* A, const T* B, T* C, int N, int bs) {
    std::fill(C, C + static_cast<std::size_t>(N) * N, T(0));

    // TODO: sest ugnjezdenih petlji, isto kao src/kernels/tiled.cpp
    (void)A; (void)B; (void)bs;
}

template <typename T>
double bench(int N, int bs, int reps = 5) {
    const std::size_t n2 = static_cast<std::size_t>(N) * N;
    std::vector<T> A(n2), B(n2), C(n2, T(0));
    std::mt19937 rng(7);
    std::uniform_real_distribution<double> d(-1.0, 1.0);
    for (std::size_t i = 0; i < n2; ++i) { A[i] = T(d(rng)); B[i] = T(d(rng)); }

    tiled<T>(A.data(), B.data(), C.data(), N, bs);   // warmup

    std::vector<double> t;
    for (int r = 0; r < reps; ++r) {
        auto s = std::chrono::steady_clock::now();
        tiled<T>(A.data(), B.data(), C.data(), N, bs);
        auto e = std::chrono::steady_clock::now();
        t.push_back(std::chrono::duration<double, std::milli>(e - s).count());
    }
    std::sort(t.begin(), t.end());

    double checksum = 0.0;                            // sprecava mrtav kod
    for (std::size_t i = 0; i < n2; ++i) checksum += double(C[i]);
    if (checksum == 1234.5678) std::fprintf(stderr, " ");

    return (2.0 * N * N * N) / (t[t.size() / 2] * 1e6);   // GFLOPS
}

int main(int argc, char** argv) {
    const int N = (argc > 1) ? std::atoi(argv[1]) : 1024;
    const int sizes[] = {8, 16, 24, 32, 48, 64, 96, 128};

    std::printf("N=%d\n%-6s %10s %10s\n", N, "bs", "double", "float");
    double best_d = 0, best_f = 0;
    int    bd = 0, bf = 0;

    for (int bs : sizes) {
        double gd = bench<double>(N, bs);
        double gf = bench<float>(N, bs);
        std::printf("%-6d %10.3f %10.3f\n", bs, gd, gf);
        if (gd > best_d) { best_d = gd; bd = bs; }
        if (gf > best_f) { best_f = gf; bf = bs; }
    }

    std::printf("\noptimum double: bs=%d (%.2f GFLOPS)\n", bd, best_d);
    std::printf("optimum float : bs=%d (%.2f GFLOPS)\n", bf, best_f);
    std::printf("odnos bs_float/bs_double = %.2f   (predvidjanje: 1.41)\n",
                bd ? double(bf) / bd : 0.0);
    return 0;
}
