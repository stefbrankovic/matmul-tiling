# Dnevnik rada

> Vodi se DOK radis, ne posle. Ovo je materijal za README i za intervju.
> **Pravilo: predvidjanje se upisuje PRE merenja. Bez izuzetka.**
> Zasto: kad zapises "ocekujem 4x" pa dobijes 6.8x, mozak registruje
> iznenadjenje i to ostane. Kad prvo vidis broj, sve deluje ocekivano
> unazad i nista se ne zalepi.

---

## 0. Masina (popuni iz `./scripts/collect_machine.sh`)

- CPU: Intel(R) Core(TM) i3-1005G1 CPU @ 1.20GHz
- L1d: 48 KB po jezgru, asocijativnost 12, cache line 64 B
- L2: 512 KB po jezgru, L3: 4 MB
- Fizickih jezgara: 2, niti: 4
- Takt: 1.20 GHz (bazični)
- g++ 15.2.0, OS Ubuntu 26.04 LTS (WSL2)

**Peak GFLOPS (procena):**
```
peak = takt_GHz x jezgra x FLOP/ciklus
     = 1.20 x 2 x 2 = **4.8 GFLOPS**
   (skalarni double bez SIMD-a: ~2 FLOP/ciklus/jezgro)
```
**Izmerena propusnost (STREAM triad):** 10.79 GB/s
**Ridge point** (peak GFLOPS/bandwidth): 0.44 FLOP/bajt

---

## 1. Predvidjanja (upisano PRE bilo kakvog merenja)

**Datum/vreme:** 07.09.2026. 11h

### 1.1. ikj vs ijk
Ocekivano ubrzanje: 3-5x
>Obrazlozenje u jednoj recenici: Kod ijk, unutrasnaj petlja pristupa B po koloni (B[k][j]), pa se skace kroz memoriju za po N mesta u svkoam koraku (**stride-N access**). Kod ikj, unutrsnja petlja ide po j, pa se elementima B i C pristupa sekvencijalno, reedom u memorji.

### 1.2. Optimalni block size
```
3 * BS^2 * 8 <= L1_bytes
BS <= sqrt(L1_bytes / 24) = 45.25
```
Predvidjanje: **BS = 32**

### 1.3. Tiled vs ikj
Ocekivano dodatno ubrzanje: 1.5-2x

### 1.4. Skaliranje na T niti
Ocekivano: 1.3-1.6x na 2 jezgara. Zasto ne linearno: 
>Jezgra dele L3 keš i memorijsku magistralu ka DRAM-u, pa se stvara usko grlo pri pristupu memoriji.

### 1.5. Jaz do OpenBLAS-a (jednonitno!)
Ocekivano: 2-4x sporije od BLAS-a

---

## 2. Izmereno

Referenca (naive baseline na N=1024): 0.376 GFLOPS (vreme: 5718 ms)

| Faza | Ocekivano | Izmereno | Razlika i objasnjenje |
|---|---|---|---|
| ikj vs naive | 3-5x | | |
| tiled vs ikj | 1.5-2x | | |
| optimalni BS | BS=32 | | |
| parallel (T niti) | 1.3-1.6x | | |
| jaz do BLAS-a (1 nit) | 2-4x sporije | | |
| -O3 -march=native | | | |

---

## 3. Mikro-eksperimenti (vidi `docs/ucenje.md`, sekcija 4)

Pusti sa `./scripts/run_experiments.sh`.

### E1 — tiled sa bs = N
Ocekivano: ______   Izmereno: ______
Da li se poklapa sa ikj? Ako ne, sta to znaci? ______

### E2 — tiled sa bs = 4
Izmereno: ______
Zasto je sporo iako blok savrseno staje u L1? ______

### E3 — N=1024 vs N=1023
GFLOPS na 1024: ______   na 1023: ______   na 1025: ______
Objasnjenje (konfliktni promasaji, asocijativnost): ______

### E4 — ijk nad transponovanim B
GFLOPS: ______ (naive ______, ikj ______)
**Zasto ijk-nad-B^T i ikj daju slican rezultat iz razlicitih razloga?**
______
**Sta transponovanje kosta i zasto se ipak isplati?**
______

### E5 — float vs double (opciono, utorak)
Optimalni BS za float: ______ (predvidjanje: ______)
Zasto je drugaciji? ______

---

## 4. Iznenadjenja i greske

- 
- 

## 5. Usmeni test (utorak uvece, bez gledanja u repo)

- [ ] Zasto je ikj brze od ijk?
- [ ] Sta tacno tiling postize?
- [ ] Kako si izabrao block size?
- [ ] Zasto skaliranje na 4 niti nije 4x?
- [ ] Zasto gubis od OpenBLAS-a?
- [ ] Sta je aritmeticki intenzitet i gde ti je kernel na roofline dijagramu?
- [ ] Sta se menja za float?
- [ ] Sta su konfliktni promasaji?
- [ ] Zasto -O3 -march=native menja odnose?
- [ ] Kako se prenosi na GPU? A na Tenstorrent?

Gde sam zamucao: ______

## 6. Sta bih uradio sa vise vremena

- 
