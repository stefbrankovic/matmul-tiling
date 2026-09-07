// src/kernels/naive.cpp  --  ijk, referentna (i najsporija) verzija
void matmul_naive(const double* A, const double* B, double* C, int N, int /*bs*/) {
    for (int i = 0; i < N; ++i)
        for (int j = 0; j < N; ++j) {
            double sum = 0.0;
            for (int k = 0; k < N; ++k)
                sum += A[i * N + k] * B[k * N + j];
            C[i * N + j] = sum;
        }
}
