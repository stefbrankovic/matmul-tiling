#!/usr/bin/env python3
"""GFLOPS vs N + block-size sweep -> results/*.png"""
import pandas as pd
import matplotlib
matplotlib.use("Agg")
import matplotlib.pyplot as plt

PREDICTED_BS = 32   # <-- tvoje predvidjanje iz velicine L1 (sekcija 3.5)

# 1. GFLOPS vs N
df = pd.read_csv("results/results.csv")
fig, ax = plt.subplots(figsize=(7, 4.5))
for v, g in df.groupby("version"):
    g = g.sort_values("N")
    ax.plot(g["N"], g["gflops"], marker="o", label=v)
ax.set_xscale("log", base=2)
ax.set_xlabel("Matrix size N")
ax.set_ylabel("GFLOPS")
ax.set_title("Throughput vs matrix size (double, -O2)")
ax.legend()
ax.grid(alpha=.3)
fig.tight_layout()
fig.savefig("results/gflops_vs_n.png", dpi=150)
print("-> results/gflops_vs_n.png")

# 2. Block size sweep
sw = pd.read_csv("results/sweep.csv")
fig, ax = plt.subplots(figsize=(7, 4.5))
ax.plot(sw["block_size"], sw["gflops"], marker="o")
best = sw.loc[sw["gflops"].idxmax()]
ax.axvline(PREDICTED_BS, ls="--", c="grey", label=f"predicted from L1 size (BS={PREDICTED_BS})")
ax.axvline(best["block_size"], ls=":", c="crimson",
           label=f"measured optimum (BS={int(best['block_size'])})")
ax.set_xscale("log", base=2)
ax.set_xlabel("Block size")
ax.set_ylabel("GFLOPS")
ax.set_title(f"Block size sweep (N={int(sw['N'].iloc[0])})")
ax.legend()
ax.grid(alpha=.3)
fig.tight_layout()
fig.savefig("results/block_size_sweep.png", dpi=150)
print(f"-> results/block_size_sweep.png   (measured optimum BS={int(best['block_size'])})")
