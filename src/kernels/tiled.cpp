// src/kernels/tiled.cpp  --  FAZA 3: cache blocking
//
// ==========================================================================
//  PRE KODA: PAPIR, 10 MINUTA
//  Nacrtaj matricu 8x8 podeljenu na blokove 4x4. Fiksiraj jedan blok (ii,jj)
//  matrice C. Koji blokovi A i B su ti potrebni da ga izracunas do kraja?
//  (odgovor: cela blok-vrsta A i cela blok-kolona B -- otuda petlja po kk)
//
//  Zatim, unutar jednog para blokova: za fiksno i i k, sta unutrasnja petlja
//  po j dodiruje? Odgovor mora da glasi "jedan red bloka B i jedan red bloka
//  C, oba sekvencijalno". Kad to mozes da kazes naglas, pisi kod.
//
//  I zapisi predvidjanje BS-a u docs/notes.md PRE nego sto pustis sweep.
//      3 * BS^2 * 8 <= L1_bytes   ->   BS <= sqrt(L1_bytes / 24)
// ==========================================================================
//
// SPECIFIKACIJA:
//   1. Spoljne petlje po BLOKOVIMA: ii, kk, jj -- korak bs
//   2. Unutrasnje po ELEMENTIMA unutar bloka: i, k, j
//   3. Granice: i_max = std::min(ii + bs, N)  -- N nije uvek deljivo sa bs!
//   4. Redosled unutrasnjih petlji: i -> k -> j (isto kao Faza 2)
//   5. C se nulira na pocetku
//
// HINT 1 (posle 20 min): skelet je
//         for ii; for kk; for jj;
//           for i = ii..imax; for k = kk..kmax; for j = jj..jmax;
//             C[i*N+j] += A[i*N+k] * B[k*N+j];
//         Telo je identicno kao u ikj -- menjaju se samo granice petlji.
// HINT 2 (posle 35 min): tri najcesce greske
//         (a) A[ii*N + k] umesto A[i*N + k]  -- blokovski indeks u telu
//         (b) for (int j = 0; j < bs; ++j)   -- mora od jj do min(jj+bs, N)
//         (c) zaboravljeno nuliranje C
//
// SANITY TEST koji vredi vise od debuggera:
//   ./build/matmul 512 tiled 512 --verify   -> mora da radi kao ikj po brzini
//   ./build/matmul 500 tiled  32 --verify   -> N nije deljivo sa bs, hvata (b)
//   ./build/matmul   8 tiled   4 --verify   -> gresku vidis rucno

#include <algorithm>
#include <cstddef>

void matmul_tiled(const double* A, const double* B, double* C, int N, int bs) {
    std::fill(C, C + static_cast<std::size_t>(N) * N, 0.0);
    if (bs <= 0) bs = 32;

    // TODO: sest ugnjezdenih petlji ovde
    (void)A; (void)B;
}
