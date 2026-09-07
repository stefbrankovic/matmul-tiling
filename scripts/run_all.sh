#!/usr/bin/env bash
# Sve verzije x sve velicine -> results/results.csv
set -e
BIN=./build/matmul
OUT=results/results.csv
BS=${BS:-32}
mkdir -p results
echo "N,version,block_size,median_ms,min_ms,max_ms,gflops" > "$OUT"

for N in 128 256 512 1024 2048; do
  for v in naive ikj tiled parallel blas; do
    # naive na N=2048 traje ~30 s po prolazu -- jedan prolaz je dovoljan
    if [ "$v" = "naive" ] && [ "$N" -ge 2048 ]; then
      EXTRA="--reps 1 --warmup 1"
    else
      EXTRA=""
    fi
    echo "  $v N=$N ..." >&2
    $BIN "$N" "$v" "$BS" --csv $EXTRA >> "$OUT" || echo "  (preskoceno: $v N=$N)" >&2
  done
done
echo "-> $OUT"
column -s, -t < "$OUT"
