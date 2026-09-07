// src/kernels/parallel.cpp  --  FAZA 5: tiled + OpenMP
//
// ==========================================================================
//  PRE KODA: PITANJE NA KOJE MORAS DA ODGOVORIS
//  Zasto je bezbedno paralelizovati po ii i jj, a katastrofalno po kk?
//  Odgovor u jednoj recenici: blokovi C[ii][jj] su nezavisni izlazi, dok
//  bi razliciti kk pisali u ISTI C[i][j] iz vise niti -- race condition,
//  i rezultat je nedeterministicki pogresan (verify ce ponekad proci,
//  ponekad ne -- to je najgora vrsta buga).
// ==========================================================================
//
// SPECIFIKACIJA:
//   1. Kopiraj OVDE svoju tiled implementaciju
//   2. #pragma omp parallel for collapse(2) schedule(static)  nad ii i jj
//   3. NIKAD nad kk
//   4. Nuliranje C radi PRE paralelnog regiona
//
// collapse(2) spaja ii i jj u jedan iteracioni prostor -- bitno kad je
// N/bs mali u odnosu na broj niti (npr. N=512, bs=64 -> samo 8 blok-vrsta).
//
// MERENJE:
//   export OMP_PROC_BIND=close OMP_PLACES=cores
//   ./scripts/run_threads.sh 1024 <tvoj_bs>
// Skaliranje NECE biti linearno. Objasnjenje zasto vredi vise od broja:
// propusnost memorije je deljeni resurs, hyperthread nije fizicko jezgro.

#include <algorithm>
#include <cstddef>
#ifdef _OPENMP
#include <omp.h>
#endif

void matmul_parallel(const double* A, const double* B, double* C, int N, int bs) {
    std::fill(C, C + static_cast<std::size_t>(N) * N, 0.0);
    if (bs <= 0) bs = 32;

    // TODO: tiled kod + pragma
    (void)A; (void)B;
}
