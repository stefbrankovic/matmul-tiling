#!/usr/bin/env bash
# Hardverski brojaci po verziji -> results/perf_raw.txt
set -e
BIN=./build/matmul
N=${1:-1024}
BS=${2:-32}
OUT=results/perf_raw.txt
mkdir -p results
: > "$OUT"
for v in naive ikj tiled parallel; do
  echo "=== $v (N=$N, bs=$BS) ===" | tee -a "$OUT"
  perf stat -e cache-references,cache-misses,L1-dcache-loads,L1-dcache-load-misses,instructions,cycles \
    $BIN "$N" "$v" "$BS" --single 2>&1 | tail -n 20 | tee -a "$OUT"
  echo | tee -a "$OUT"
done
echo "-> $OUT  (prepisi u results/perf_counters.md kao tabelu)"
