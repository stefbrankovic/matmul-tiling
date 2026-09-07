#!/usr/bin/env python3
"""
Roofline dijagram.

  python scripts/plot_roofline.py --peak 25.6 --bw 18.4 [--n 1024] [--bs 32]

  --peak : vrsni GFLOPS masine  = takt(GHz) x jezgra x FLOP/ciklus
           (skalarni double bez SIMD-a: ~2 FLOP/ciklus po jezgru;
            sa AVX2 FMA: 16 FLOP/ciklus po jezgru)
  --bw   : izmerena propusnost (GB/s) iz `./build/matmul 50 bandwidth`

Aritmeticki intenzitet je MODEL (sekcija 3.6), ne merenje -- reci to u README-u.
Ako imas perf brojace, mozes ga izmeriti: AI = 2*N^3 / (LLC-load-misses * 64).
"""
import argparse
import numpy as np
import pandas as pd
import matplotlib
matplotlib.use("Agg")
import matplotlib.pyplot as plt

p = argparse.ArgumentParser()
p.add_argument("--peak", type=float, required=True, help="peak GFLOPS")
p.add_argument("--bw",   type=float, required=True, help="STREAM bandwidth GB/s")
p.add_argument("--n",    type=int, default=1024)
p.add_argument("--bs",   type=int, default=32)
a = p.parse_args()

df = pd.read_csv("results/results.csv")
df = df[df["N"] == a.n].set_index("version")["gflops"].to_dict()

# Modelovani aritmeticki intenzitet (FLOP/bajt)
ai = {
    "naive":    2 / 64,          # jedna cache linija po 2 FLOP-a (B je column-wise)
    "ikj":      2 / 24,          # tri unit-stride toka
    "tiled":    a.bs / 12,       # 2*BS^3 / (3*BS^2*8)
    "parallel": a.bs / 12,
}

x = np.logspace(-2, 2, 400)
roof = np.minimum(a.peak, a.bw * x)
ridge = a.peak / a.bw

fig, ax = plt.subplots(figsize=(7, 4.5))
ax.loglog(x, roof, "k-", lw=2, label=f"roofline (peak {a.peak:g} GFLOPS, {a.bw:g} GB/s)")
ax.axvline(ridge, ls=":", c="grey")
ax.text(ridge * 1.1, a.peak * 0.12, f"ridge point\nAI={ridge:.2f}", fontsize=8, color="grey")

for v, intensity in ai.items():
    if v in df:
        ax.plot(intensity, df[v], "o", ms=9, label=f"{v} ({df[v]:.2f} GFLOPS)")
        ax.annotate(v, (intensity, df[v]), textcoords="offset points",
                    xytext=(6, 6), fontsize=9)

ax.set_xlabel("Arithmetic intensity (FLOP/byte)")
ax.set_ylabel("Performance (GFLOPS)")
ax.set_title(f"Roofline, N={a.n} (double, -O2)")
ax.grid(alpha=.3, which="both")
ax.legend(fontsize=8, loc="lower right")
fig.tight_layout()
fig.savefig("results/roofline.png", dpi=150)
print("-> results/roofline.png")
