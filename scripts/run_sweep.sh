#!/usr/bin/env bash
# Block-size sweep za tiled -> results/sweep.csv
set -e
BIN=./build/matmul
OUT=results/sweep.csv
N=${1:-1024}
mkdir -p results
echo "N,block_size,median_ms,gflops" > "$OUT"
for BS in 8 16 24 32 48 64 96 128 256; do
  echo "  bs=$BS ..." >&2
  $BIN "$N" tiled "$BS" --csv-sweep >> "$OUT"
done
echo "-> $OUT"
column -s, -t < "$OUT"
