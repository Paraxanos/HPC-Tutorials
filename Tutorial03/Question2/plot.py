#!/usr/bin/env python3
"""Q2 graphs: threads vs time / speedup / efficiency, from run.sh's CSVs."""
import csv
import matplotlib
matplotlib.use("Agg")
import matplotlib.pyplot as plt


def load(path):
    with open(path) as f:
        return [{k: float(v) for k, v in row.items()} for row in csv.DictReader(f)]


ser, par = load("q2_serial.csv")[0], load("q2_parallel.csv")
threads = [int(r["threads"]) for r in par]
times = [r["time"] for r in par]
t_ser, n_img = ser["time"], int(ser["n"])

for name, kind, ys, ylabel in [
    ("q2_threads_vs_time.png", "time", times, "execution time (s)"),
    ("q2_threads_vs_speedup.png", "speedup", [t_ser / t for t in times],
     r"speedup  $T_{serial}/T_p$"),
    ("q2_threads_vs_efficiency.png", "efficiency", [t_ser / t / p for t, p in zip(times, threads)],
     r"efficiency  $S_p/p$"),
]:
    fig, ax = plt.subplots(figsize=(7, 4.5))
    ax.plot(threads, ys, "o-", color="tab:green", label=f"{n_img}x{n_img} image")
    if kind == "speedup":
        ax.plot(threads, threads, "k:", label="ideal")
    if kind == "efficiency":
        ax.axhline(1.0, color="k", ls=":", label="ideal"); ax.set_ylim(0, 1.2)
    ax.set_xlabel("threads"); ax.set_xticks(threads); ax.set_ylabel(ylabel)
    ax.set_title(f"Q2: threads vs {kind}"); ax.grid(alpha=.3); ax.legend(fontsize=8)
    fig.tight_layout(); fig.savefig(name, dpi=130); plt.close(fig); print("wrote", name)
