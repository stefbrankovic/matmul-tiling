// src/kernels/reorder.cpp  --  FAZA 2: ikj
//
// ==========================================================================
//  PRE KODA: PAPIR, 5 MINUTA
//  Nacrtaj matricu 8x8. Za naivnu petlju (i -> j -> k) prstom prati koje
//  elemente unutrasnja petlja po k dodiruje:
//      A[i*N+k]  ide  ->  ->  ->     (jedan red, korak 1)
//      B[k*N+j]  ide  |   |   |      (jedna kolona, korak N)
//  Sada zameni redosled na i -> k -> j i ponovi crtanje. Sta se promenilo?
//  Kad umes da izgovoris odgovor naglas, pisi kod. Ne pre.
// ==========================================================================
//
// SPECIFIKACIJA:
//   1. Redosled petlji: i -> k -> j
//   2. C se mora NULIRATI na pocetku (vec je uradjeno dole), jer sada
//      akumuliras (+=), ne dodeljujes (=).
//   3. Iz unutrasnje petlje izvuci A[i*N + k] u lokalnu const promenljivu --
//      pomazes kompajleru da ga drzi u registru.
//   4. Unutrasnja petlja ide po j: i B[k*N+j] i C[i*N+j] su unit-stride.
//
// HINT 1 (posle 10 min): telo unutrasnje petlje je tacno jedna linija oblika
//         C[?] += a * B[?];   -- pitanje je samo sta je ? u oba slucaja.
// HINT 2 (posle 20 min): najcesca greska je C[i*N + k] umesto C[i*N + j].
//         Indeks izlaza NIKAD ne sadrzi k -- k je dimenzija po kojoj sabiras.
//
// PROVERA:  ./build/matmul 512 ikj --verify
// AKO PADNE: ./build/matmul 8 ikj --verify   (na 8x8 gresku vidis odmah)

#include <algorithm>
#include <cstddef>

void matmul_ikj(const double* A, const double* B, double* C, int N, int /*bs*/) {
    std::fill(C, C + static_cast<std::size_t>(N) * N, 0.0);

    // TODO: tvoj kod ovde
    (void)A; (void)B;
}
