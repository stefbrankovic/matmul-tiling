#!/usr/bin/env bash
# Skaliranje po broju niti -> results/threads.csv
set -e
BIN=./build/matmul
OUT=results/threads.csv
N=${1:-1024}
BS=${2:-32}
mkdir -p results
echo "threads,N,block_size,median_ms,gflops" > "$OUT"
for T in 1 2 4 8; do
  echo "  threads=$T ..." >&2
  OMP_NUM_THREADS=$T $BIN "$N" parallel "$BS" --csv-sweep \
    | awk -v t=$T -F, '{print t","$1","$2","$3","$4}' >> "$OUT"
done
echo "-> $OUT"
column -s, -t < "$OUT"
