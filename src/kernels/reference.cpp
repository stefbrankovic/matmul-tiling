// src/kernels/reference.cpp  --  OpenBLAS referentni plafon
#ifdef HAVE_BLAS
#if __has_include(<cblas.h>)
  #include <cblas.h>
#elif __has_include(<openblas/cblas.h>)
  #include <openblas/cblas.h>
#else
  #error "cblas.h nije nadjen -- instaliraj libopenblas-dev ili iskljuci BLAS"
#endif

void matmul_blas(const double* A, const double* B, double* C, int N, int /*bs*/) {
    cblas_dgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans,
                N, N, N, 1.0, A, N, B, N, 0.0, C, N);
}
#else
#include <cstdio>
#include <cstdlib>
void matmul_blas(const double*, const double*, double*, int, int) {
    std::fprintf(stderr, "BLAS nije dostupan (rebuild sa libopenblas-dev)\n");
    std::exit(2);
}
#endif
