// src/kernels/transposed.cpp  --  EKSPERIMENT: ijk nad transponovanim B
//
// ==========================================================================
//  OVO JE NAJVREDNIJI EKSPERIMENT U CELOM PROJEKTU, a kosta 15 minuta.
//
//  Teza koju testiras: poenta nije redosled petlji. Poenta je KORAK KROZ
//  MEMORIJU. Ako transponujes B unapred, originalni ijk redosled odjednom
//  postaje brz -- jer B[j*N+k] sada ide unit-stride po k.
//
//  Kad izmeris, odgovori naglas: zasto ijk-nad-B^T i ikj daju slican
//  rezultat, iako rade potpuno razlicite stvari? (Odgovor: oba pretvaraju
//  skokoviti tok u sekvencijalni; prvi menja PODATKE, drugi menja PETLJE.)
//
//  I drugo pitanje: sta transponovanje kosta, i zasto se ipak isplati?
//  (O(N^2) rada da bi se popravilo O(N^3) pristupa)
// ==========================================================================
//
// SPECIFIKACIJA:
//   1. Alociraj Bt velicine N*N (std::vector<double>)
//   2. Bt[j*N + k] = B[k*N + j]
//   3. Pusti ORIGINALNI ijk redosled, ali citaj Bt[j*N + k] umesto B[k*N + j]
//   4. Transponovanje se MERI zajedno sa mnozenjem -- to je posteno
//
// PROVERA:  ./build/matmul 512 ijkT --verify
// (opciono -- ako kasnis, ostavi TODO i preskoci; verify ce javiti FAIL)

#include <vector>
#include <cstddef>

void matmul_ijk_transposed(const double* A, const double* B, double* C, int N, int /*bs*/) {
    // TODO: tvoj kod ovde
    (void)A; (void)B; (void)C; (void)N;
}
