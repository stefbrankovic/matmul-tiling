# matmul-tiling

Dense matrix multiplication in C++, taken from a naive triple loop to a
cache-blocked, multi-threaded kernel — with every step measured.

**Every version performs exactly the same number of floating-point
operations.** The ~__× difference in runtime comes entirely from where the
data is when it is needed.

![GFLOPS vs N](results/gflops_vs_n.png)

## Results (N = 1024, double precision, -O2)

| Version                | GFLOPS | Speedup | L1 miss rate | Notes |
|------------------------|--------|---------|--------------|-------|
| naive (ijk)            | _      | 1.0×    | _%           | B traversed column-wise |
| loop reorder (ikj)     | _      | _×      | _%           | two sequential streams |
| cache-blocked (BS=_)   | _      | _×      | _%           | working set fits L1 |
| + OpenMP (_ threads)   | _      | _×      | _%           | |
| OpenBLAS (reference)   | _      | _×      | —            | packing + SIMD + µkernels |

## What the numbers say

**Loop order alone gives _×.** In `ijk`, the inner loop walks `B` down a
column while the array is stored row-major, so the stride is `N·8` bytes:
every access touches a new cache line and 56 of every 64 bytes fetched are
discarded. Reordering to `ikj` turns both `B` and `C` into unit-stride
streams the prefetcher can follow.

**The optimal block size was predicted before measuring.** With a __ KB L1
and three `double` blocks resident, `3·BS²·8 ≤ _____` gives `BS ≤ __`, so
__ was the prediction. Measured optimum: **_**. [poklopilo se / evo zašto ne]

![block size sweep](results/block_size_sweep.png)

**Parallel scaling is sub-linear** — _× on _ physical cores. Memory
bandwidth is a shared resource, so once the kernel is bandwidth-limited,
additional threads compete for the same bus rather than adding throughput.

**The gap to OpenBLAS is _×**, and it is not a mystery: BLAS packs operands
into contiguous buffers, uses hand-tuned SIMD micro-kernels, and blocks for
several cache levels at once. This project stops at one level of blocking
on purpose.

## Roofline

![roofline](results/roofline.png)

Arithmetic intensity rises from ≈0.03 FLOP/byte (naive) to ≈_ FLOP/byte
(blocked, BS=_), moving the kernel off the memory-bandwidth roof and
toward the compute roof. Machine peak estimated at _ GFLOPS (clock × cores ×
FLOP/cycle); measured STREAM-triad bandwidth _ GB/s. Arithmetic intensity is
a model estimate, not a counter reading — see `scripts/plot_roofline.py`.

## Cache counters

| Version  | L1-dcache-load-misses | miss rate | IPC |
|----------|-----------------------|-----------|-----|
| naive    | _                     | _%        | _   |
| ikj      | _                     | _%        | _   |
| tiled    | _                     | _%        | _   |

Full output in [results/perf_counters.md](results/perf_counters.md).

## Method

Median of 7 timed runs after 3 warmup runs; min and max also reported.
Every kernel verified against the naive implementation (max abs diff
< 1e-9). Main comparison compiled with `-O2`; `-O3 -march=native` measured
separately — see [results/](results/), where the ranking changes because _.

**Machine:** _CPU_ · L1d _KB_ · L2 _KB_ · L3 _MB_ · _ physical cores ·
g++ _ · _OS_. Measured on AC power with background load minimised.

## Build and run

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release && cmake --build build -j
./scripts/collect_machine.sh
./scripts/run_all.sh
./scripts/run_sweep.sh
./scripts/run_threads.sh
./scripts/run_perf.sh
python scripts/plot.py
python scripts/plot_roofline.py --peak <GFLOPS> --bw <GB/s>
```

## Limitations

Single level of blocking; no operand packing, no SIMD intrinsics, no
NUMA awareness. Double precision only. Square matrices only — see the
roadmap for non-square shapes taken from a real model.

## Roadmap

- Non-square shapes (M ≠ N ≠ K) taken from a real inference profile
- CUDA port: shared-memory tiling, coalesced access, occupancy
- Operand packing and SIMD micro-kernels

## Related

[inference-bench](link) profiles where a neural network actually spends
its time. Convolutions and attention both lower to GEMM — this repo is
what happens one level below that measurement.

## License

MIT
