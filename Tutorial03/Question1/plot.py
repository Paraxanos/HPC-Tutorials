#!/usr/bin/env python3
"""Q1 graphs: points vs pi, points vs error, and threads vs time / speedup /
efficiency, from the CSVs written by run.sh."""
import csv, math
import matplotlib
matplotlib.use("Agg")
import matplotlib.pyplot as plt


def load(path):
    with open(path) as f:
        return [{k: float(v) for k, v in row.items()} for row in csv.DictReader(f)]


ser, par = load("q1_serial.csv"), load("q1_parallel.csv")
threads = sorted({int(r["threads"]) for r in par})
points = sorted({int(r["points"]) for r in par})
t_ser = {int(r["points"]): r["time"] for r in ser}
t_par = {(int(r["threads"]), int(r["points"])): r["time"] for r in par}


def save(fig, name):
    fig.tight_layout(); fig.savefig(name, dpi=130); plt.close(fig); print("wrote", name)


# points vs estimated pi
fig, ax = plt.subplots(figsize=(7, 4.5))
for p in threads:
    ax.plot(points, [next(r["pi"] for r in par if int(r["threads"]) == p and int(r["points"]) == n)
                     for n in points], "o-", ms=4, label=f"{p} threads")
ax.plot(points, [r["pi"] for r in ser], "k^--", ms=5, label="serial")
ax.axhline(math.pi, color="r", ls=":", label=r"$\pi$")
ax.set_xscale("log"); ax.set_xlabel("number of points"); ax.set_ylabel(r"estimated $\pi$")
ax.set_title(r"Q1: number of points vs estimated $\pi$"); ax.grid(alpha=.3); ax.legend(fontsize=8)
save(fig, "q1_points_vs_pi.png")

# points vs error
fig, ax = plt.subplots(figsize=(7, 4.5))
for p in threads:
    ax.plot(points, [max(next(r["error"] for r in par
                              if int(r["threads"]) == p and int(r["points"]) == n), 1e-12)
                     for n in points], "o-", ms=4, label=f"{p} threads")
ax.plot(points, [1 / math.sqrt(n) for n in points], "k:", label=r"$1/\sqrt{N}$ reference")
ax.set_xscale("log"); ax.set_yscale("log")
ax.set_xlabel("number of points"); ax.set_ylabel(r"absolute error $|\hat\pi-\pi|$")
ax.set_title("Q1: number of points vs error"); ax.grid(alpha=.3, which="both"); ax.legend(fontsize=8)
save(fig, "q1_points_vs_error.png")

# threads vs time / speedup / efficiency, one curve per problem size
big = [n for n in points if n >= 10**6]
for name, kind, ylabel in [
    ("q1_threads_vs_time.png", "time", "execution time (s)"),
    ("q1_threads_vs_speedup.png", "speedup", r"speedup  $T_{serial}/T_p$"),
    ("q1_threads_vs_efficiency.png", "efficiency", r"efficiency  $S_p/p$"),
]:
    fig, ax = plt.subplots(figsize=(7, 4.5))
    for n in big:
        ts = [t_par[(p, n)] for p in threads]
        ys = (ts if kind == "time" else
              [t_ser[n] / t for t in ts] if kind == "speedup" else
              [t_ser[n] / t / p for t, p in zip(ts, threads)])
        ax.plot(threads, ys, "o-", label=f"N = 1e{int(math.log10(n))}")
    if kind == "time":
        ax.set_yscale("log")
    if kind == "speedup":
        ax.plot(threads, threads, "k:", label="ideal")
    if kind == "efficiency":
        ax.axhline(1.0, color="k", ls=":", label="ideal"); ax.set_ylim(0, 1.2)
    ax.set_xlabel("threads"); ax.set_xticks(threads); ax.set_ylabel(ylabel)
    ax.set_title(f"Q1: threads vs {kind}"); ax.grid(alpha=.3); ax.legend(fontsize=8)
    save(fig, name)
