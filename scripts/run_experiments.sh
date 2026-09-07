#!/usr/bin/env bash
# Mikro-eksperimenti iz docs/ucenje.md, sekcija 4.
# Kostaju 7 minuta i svaki daje po jednu recenicu za README.
set -e
BIN=./build/matmul
BS=${BS:-32}
N=${N:-1024}

echo "############ E1: tiled sa bs = N  (mora da bude ~= ikj) ############"
echo "-- referenca:"
$BIN "$N" ikj
echo "-- tiled sa bs=$N (ceo problem je jedan blok -> degenerise u ikj):"
$BIN "$N" tiled "$N"
echo
echo "############ E2: tiled sa bs = 4  (rezija petlji nadjaca lokalnost) ############"
$BIN "$N" tiled 4
echo "-- poredjenje sa optimalnim bs=$BS:"
$BIN "$N" tiled "$BS"
echo
echo "############ E3: stepen dvojke vs neparno N (konfliktni promasaji) ############"
for n in 1023 1024 1025; do
  $BIN "$n" tiled "$BS"
done
echo "-- isto za ikj (bez blokiranja efekat je cesto jaci):"
for n in 1023 1024 1025; do
  $BIN "$n" ikj
done
echo
echo "############ E4: ijk nad transponovanim B ############"
echo "-- ako javi FAIL/nule, jos nisi napisao src/kernels/transposed.cpp"
$BIN 512 ijkT --verify || true
$BIN "$N" naive
$BIN "$N" ikj
$BIN "$N" ijkT
echo
echo "Upisi rezultate u docs/notes.md, sekcija 3."
